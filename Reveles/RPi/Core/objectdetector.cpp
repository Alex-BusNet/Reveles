#include "objectdetector.h"
#include "Common/logger.h"
#include "Common/messages.h"

// Convert to string
#define SSTR( x ) static_cast<std::ostringstream &>((std::ostringstream() << std::dec << x )).str()

Q_GLOBAL_STATIC(ObjectDetector, od)

ObjectDetector::ObjectDetector()
{
    /*
     * Empty C'tor needed for Q_GLOBAL_STATIC macro.
     * Place anything in the Init() function that would
     * normally go in the c'tor.
     */
}

ObjectDetector *ObjectDetector::instance()
{
    return od;
}

void ObjectDetector::Init()
{
    instance()->setObjectName("ObjectDetector");

    for (int i = 0; i < 16; i++)
    {
        objects[i] = ObjContainer{ Point(0,0), false, -1, NO_STATE };
    }


    if(!lowerBody.load("Data/haarcascade_lowerbody.xml"))
    {
        Logger::writeLine(instance(), Reveles::HAAR_LOWER_BODY_LOAD_FAIL);
    }

    if(!fullBody.load("Data/haarcascade_fullbody.xml"))
    {
        Logger::writeLine(instance(), Reveles::HAAR_FULL_BODY_LOAD_FAIL);
    }

    if(!upperBody.load("Data/haarcascade_upperbody.xml"))
    {
        Logger::writeLine(instance(), Reveles::HAAR_UPPER_BODY_LOAD_FAIL);
    }

    exiting = false;
}

void ObjectDetector::TeachHOG()
{

}

void ObjectDetector::Run()
{
    // Start PeopleDetect() as concurrent process
    // For some reason, Qt is ok with using QtConcurrent::Run
    // as a lambda function, but not as a normal function call.
    future = QtConcurrent::run([=]() { PeopleDetect(); });
}

void ObjectDetector::aboutToQuit()
{
    exiting = true;
}

void ObjectDetector::PeopleDetect()
{
//    HOGDescriptor hog;
//    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
    Mat frame;

    while(RevelesCamera::instance()->ReadFrame(frame))
    {
//        DetectAndDraw(hog, frame);
        FaceFinder(frame);
#if defined OBJ_DETECT_DEBUG && defined USE_OBJ_DETECT
        imshow("Detector Output", frame);
#endif

        if (exiting)
        {
            Logger::writeLine(instance(), Reveles::OD_STOPPING);
            break;
        }
    }

    Logger::writeLine(instance(), Reveles::FRAME_READ_FAIL);
}

FutureStatus ObjectDetector::GetFutureStatus()
{
    if(future.isRunning()) { return RUNNING; }
    if(future.isPaused()) {return PAUSED; }
    if(future.isFinished()) { return FINISHED; }
    if(future.isCanceled()) { return STOPPED; }

    return NON_EXISTENT;
}

void ObjectDetector::DetectAndDraw(const HOGDescriptor &hog, Mat &img)
{
    vector<Rect>found, found_filtered;
//    double t = (double)getTickCount();

    hog.detectMultiScale(img, found, 0.0005, Size(4, 4), Size(8, 8), 1.05, 2);

//    t = (double) getTickCount() - t;

    for(size_t i = 0; i < found.size(); i++)
    {
        Rect r = found[i];
        size_t j;
        for (j = 0; j < found.size(); j++)
        {
            if( j != i && (r & found[j]) == r)
                break;
        }

        if(j == found.size())
            found_filtered.push_back(r);
    }

    for(int i = 0; i < found_filtered.size(); i++)
    {
        Rect r = found_filtered[i];

        // Draw a rectangle around the detected object.
        // HOG generates rectangles that are slightly
        // larger than the object, so we are going to shrink
        // them a bit for neatness. NOTE: this may be removed
        // in later versions. It is mostly for debugging purposes.
//        r.x += cvRound(r.width * 0.1);
//        r.width = cvRound(r.width * 0.8);
//        r.y += cvRound(r.height * 0.07);
//        r.height += cvRound(r.height * 0.8);
//        rectangle(img, r.tl(), r.br(), cv::Scalar(0, 255, 0), 3);

        // object tracking
        int centerX = r.tl().x + (r.width / 2);
        int centerY = r.tl().y + (r.height / 2);
        Point center = Point(centerX, centerY);
        Point trackedCenter = objects[i].screenLoc;

        /// TODO: Determine what zone the object is in.
        /// Note: Zones are regions of the frame.

        cout << "[ ObjectDetector ] Tracked: " << trackedCenter << " Found: " << center << " idx: " << i << endl;

//        if(trackedCenter != Point(0,0))
//        {
            int dx = center.x - trackedCenter.x;
            int dy = center.y - trackedCenter.y;

            // The deltas here will need to be adjusted
            // to compensate for the moving robot.
            // Larger deltas *should* indicate people, while
            // smaller deltas *should* indicate static objects
            if(dx > 0 && dy == 0)
            {
                objects[i].dir = L2R;
            }
            else if(dx > 0 && dy < 0)
            {
                objects[i].dir = DIAG_L2R;
            }
            else if(dx < 0 && dy == 0)
            {
                objects[i].dir = R2L;
            }
            else if(dx < 0 && dy < 0)
            {
                objects[i].dir = DIAG_R2L;
            }
            else if(dx < 0 && dy > 0)
            {
                objects[i].dir = DIAG_R2L;
            }
            else if(dx > 0 && dy > 0)
            {
                objects[i].dir = DIAG_L2R;
            }
            // TOWARDS direction detection
            //
            // AWAY direction detection

            cout << "[ ObjectDetector ] DeterminedDirection: " << objects[i].dir << endl;
            DetectionQueue::enqueue(ObjectTracking{ i, objects[i].dir, objects[i].zone });
//        }
//        else
//        {
            if(trackedCenter != center)
                objects[i].screenLoc = center;
//        }

        objects[i].updated = true;

//        circle(img, center, 2, Scalar(0,0,255), 2);
//        putText(img, SSTR(i), Point(r.tl().x + 5, r.tl().y + 10), FONT_HERSHEY_SIMPLEX, 0.25, Scalar(50, 170, 50), 1);
    }

    for(int i = 0; i < 16; i++)
    {
        if(!objects[i].updated)
        {
            objects[i] = ObjContainer{ Point(0,0), false, objects[i].zone, NO_STATE };
            DetectionQueue::enqueue(ObjectTracking{i, NO_STATE, objects[i].zone});
        }
        else
            objects[i].updated = false;

    }
}

void ObjectDetector::FaceFinder(Mat &img)
{
    Mat frame_gray;
    vector<Rect> bodies;

    cvtColor(img, frame_gray, COLOR_BGR2GRAY);
    equalizeHist(frame_gray, frame_gray);

    // May change this to lowerBody or fullBody.
    upperBody.detectMultiScale(frame_gray, bodies, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(60,60) );

    for(size_t i = 0; i < bodies.size(); i++)
    {
        Point center (bodies[i].x + bodies[i].width / 2, bodies[i].y + bodies[i].height / 2);
        rectangle(img, bodies[i].tl(), bodies[i].br(), Scalar(0, 255, 0), 2);
        circle(img, center, 2, Scalar(0,0,255), 2);
    }
}
