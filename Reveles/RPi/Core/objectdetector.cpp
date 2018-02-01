#include "objectdetector.h"

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
    for (int i = 0; i < 16; i++)
    {
        objects[i] = ObjContainer{ Point(0,0), false, -1, NO_STATE };
//        frameUpdated[i] = false;
//        objectDirection[i] = NO_STATE;
    }

    exiting = false;
}

void ObjectDetector::TeachHOG()
{

}

void ObjectDetector::Run()
{
    // Start PeopleDetect() as concurrent process
//    future = QtConcurrent::run(PeopleDetect);
}

void ObjectDetector::aboutToQuit()
{
    exiting = true;
}

void ObjectDetector::PeopleDetect()
{
    /// This needs to be altered to allow concurrent
    /// running while the rest of the engine runs.

    // For some reason, Qt is ok with using QtConcurrent::Run
    // as a lambda function, but not as a normal function call.

    QtConcurrent::run([=]()
    {
        HOGDescriptor hog;
        hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
//        namedWindow("People Detect", WINDOW_AUTOSIZE);

        VideoCapture vc(0);
        Mat frame;

        if(!vc.isOpened())
        {
            cout << "Error opening camera." << endl;
            return;
        }

        while(vc.read(frame))
        {
            frame.resize(Size(400, 300));
            DetectAndDraw(hog, frame);

            cout << ".";

//            imshow("People Detect", frame);

            // Wait breifly for ESC to be pressed.
            int k = waitKey(1);
            if (exiting || k == 27)
            {
                cout << "Stopping Object Detection." << endl;
                break;
            }
        }

        cout << "[ObjectDetector] No frame to read." << endl;

        destroyWindow("People Detect");
    });

    cout << "--" << endl;
}

void ObjectDetector::DetectAndDraw(const HOGDescriptor &hog, Mat &img)
{
    vector<Rect>found, found_filtered;
    double t = (double)getTickCount();

    hog.detectMultiScale(img, found, 0.0005, Size(4, 4), Size(8, 8), 1.05, 2);

    t = (double) getTickCount() - t;

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
        r.x += cvRound(r.width * 0.1);
        r.width = cvRound(r.width * 0.8);
        r.y += cvRound(r.height * 0.07);
        r.height += cvRound(r.height * 0.8);
//        rectangle(img, r.tl(), r.br(), cv::Scalar(0, 255, 0), 3);

        // object tracking
        int centerX = r.tl().x + (r.width / 2);
        int centerY = r.tl().y + (r.height / 2);
        Point center = Point(centerX, centerY);
        Point trackedCenter = objects[i].screenLoc;

        /// TODO: Determine what zone the object is in.
        /// Note: Zones are regions of the frame.

        if(trackedCenter != Point(0,0))
        {
            int dx = center.x - trackedCenter.x;
            int dy = center.y - trackedCenter.y;

            // The deltas here will need to be adjusted
            // to compensate for the moving robot.
            // Larger deltas *should* indicate people, while
            // smalled deltas *should* indicate static objects
            if(dx > 0 && dy == 0)
            {
                objects[i].dir = L2R;
//                objectDirection[i] = L2R;
            }
            else if(dx > 0 && dy < 0)
            {
                objects[i].dir = DIAG_L2R;
//                objectDirection[i] = DIAG_L2R;
            }
            else if(dx < 0 && dy == 0)
            {
                objects[i].dir = R2L;
//                objectDirection[i] == R2L;
            }
            else if(dx < 0 && dy < 0)
            {
                objects[i].dir = DIAG_R2L;
//                objectDirection[i] == DIAG_R2L;
            }
            else if(dx < 0 && dy > 0)
            {
                objects[i].dir = DIAG_R2L;
//                objectDirection[i] = DIAG_R2L;
            }
            else if(dx > 0 && dy > 0)
            {
                objects[i].dir = DIAG_L2R;
//                objectDirection[i] = DIAG_L2R;
            }
            // TOWARDS direction detection
            //
            // AWAY direction detection

            DetectionQueue::enqueue(ObjectTracking{ i, objects[i].dir, objects[i].zone });
        }
        else
        {
            if(trackedCenter != center)
                objects[i].screenLoc = center;
        }

        objects[i].updated = true;
//        frameUpdated[i] = true;

//        circle(img, center, 2, Scalar(0,0,255), 2);
//        putText(img, SSTR(i), Point(r.tl().x + 5, r.tl().y + 10), FONT_HERSHEY_SIMPLEX, 0.25, Scalar(50, 170, 50), 1);
    }

    for(int i = 0; i < 16; i++)
    {
//        if(!frameUpdated[i])
        if(!objects[i].updated)
        {
            objects[i] = ObjContainer{ Point(0,0), false, objects[i].zone, NO_STATE };
//            objectDirection[i] = NO_STATE;
            DetectionQueue::enqueue(ObjectTracking{i, NO_STATE, objects[i].zone});
        }
        else
            objects[i].updated = false;
//            frameUpdated[i] = false;

    }
}
