#include "analyticalengine.h"
#include <QtConcurrent/QtConcurrent>
#include <iostream>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <vector>

#define SSTR( x ) static_cast<std::ostringstream &>((std::ostringstream() << std::dec << x )).str()

using namespace  std;

Q_GLOBAL_STATIC(AnalyticalEngine, rae)

AnalyticalEngine *AnalyticalEngine::instance()
{
    return rae;
}

/*
 * Initialize variables for the analytical engine
 */
void AnalyticalEngine::Init()
{
    predictedState = NO_STATE;
    actualState = NO_STATE;
    presentState = NO_STATE;
    lastState = NO_STATE;

    QFile tmSaveData("Assets/Data/transitionMatrix.csv");
    if(!tmSaveData.open(QIODevice::ReadOnly))
    {
        cout << "Transition Matrix data not found. Using default." << endl;
    }
    else
    {
        // Read the transition matrix save data
        QByteArray tmArr = tmSaveData.readAll();
        QStringList tmSL = QString(tmArr).split(',');
        int listIdx = 0;
        for(int i = 0; i < 7; i++)
        {
            for(int j = 0; j < 7; j++)
            {
                transitionMatrix[i][j] = tmSL[listIdx++].toFloat();
            }
        }
    }

    if(tmSaveData.isOpen())
        tmSaveData.close();

    QFile dmSaveData("Assets/Data/decisionMatrix.csv");
    if(!dmSaveData.open(QIODevice::ReadOnly))
    {
        cout << "Decision Matrix data not found." << endl;
    }
    else
    {
        // Read the decision matrix save data
        QByteArray dmArr = dmSaveData.readAll();
        QStringList dmSL = QString(dmArr).split(',');
        int listIdx = 0;
        for(int i = 0; i < 7; i++)
        {
            for(int j = 0; j < 7; j++)
            {
                decisionMatrix[i][j] = dmSL[listIdx++].toFloat();
            }
        }
    }

    if(dmSaveData.isOpen())
        dmSaveData.close();

    for (int i = 0; i < 16; i++)
    {
        objects[i] = Point(0,0);
        frameUpdated[i] = false;
        objectDirection[i] = NO_STATE;
    }

}

void AnalyticalEngine::aboutToQuit()
{
    this->Save();
}

AnalyticalEngine::AnalyticalEngine()
{
    /*
     * Empty C'tor needed for Q_GLOBAL_STATIC macro.
     * Place anything in the Init() function that would
     * normally go in the c'tor.
     */
}

/*!
 * \brief Ping sensors for environment state
 */
void AnalyticalEngine::CheckEnv()
{
    pirs[0] = RevelesIO::instance()->readPIR(PIR_FL);
    pirs[1] = RevelesIO::instance()->readPIR(PIR_FR);
    pirs[2] = RevelesIO::instance()->readPIR(PIR_BL);
    pirs[3] = RevelesIO::instance()->readPIR(PIR_BR);

    uss[0] = RevelesIO::instance()->triggerUltrasonic(US_LEFT);
    uss[0] = RevelesIO::instance()->triggerUltrasonic(US_FRONT);
    uss[0] = RevelesIO::instance()->triggerUltrasonic(US_RIGHT);
    uss[0] = RevelesIO::instance()->triggerUltrasonic(US_BACK);
}

/*!
 * \brief Determine how to avoid obstacle(s)
 */
void AnalyticalEngine::ProcessEnv()
{
    /// Can this be threaded?

    // Determine if object exists

    // Determine if object is moving

    // Determine which direction the object is moving

    /// TODO: Multi obstacle avoidance handling.
    ///     Note: Some of this handling will
    ///             be able to use available map data
    ///             to determine if a short response distance
    ///             is a foreign object or a wall.
}

void AnalyticalEngine::AdjustPath_Inanimate()
{

}

/*!
 * \brief Alter path to avoid people.
 */
void AnalyticalEngine::AdjustPath_Animate()
{

}

void AnalyticalEngine::PeopleDetect()
{
    /// This needs to be altered to allow concurrent
    /// running while the rest of the engine runs.
    HOGDescriptor hog;
    hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
    namedWindow("People Detect", 1);

    VideoCapture vc(0);
    Mat frame;

    if(!vc.isOpened())
    {
        cout << "Error opening camera." << endl;
        return;
    }

    while(vc.read(frame))
    {
        DetectAndDraw(hog, frame);

        imshow("People Detect", frame);

        // Wait breifly for ESC to be pressed.
        int k = waitKey(1);
        if (k == 27)
        {
            break;
        }
    }
}

void AnalyticalEngine::DetectAndDraw(const HOGDescriptor &hog, Mat &img)
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

        r.x += cvRound(r.width * 0.1);
        r.width = cvRound(r.width * 0.8);
        r.y += cvRound(r.height * 0.07);
        r.height += cvRound(r.height * 0.8);
        rectangle(img, r.tl(), r.br(), cv::Scalar(0, 255, 0), 3);

        // object tracking
        int centerX = r.tl().x + (r.width / 2);
        int centerY = r.tl().y + (r.height / 2);
        Point center = Point(centerX, centerY);
        Point trackedCenter = objects[i];

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
                objectDirection[i] = L2R;
            }
            else if(dx > 0 && dy < 0)
            {
                objectDirection[i] = DIAG_L2R;
            }
            else if(dx < 0 && dy == 0)
            {
                objectDirection[i] == R2L;
            }
            else if(dx < 0 && dy < 0)
            {
                objectDirection[i] == DIAG_R2L;
            }
            else if(dx < 0 && dy > 0)
            {
                objectDirection[i] = DIAG_R2L;
            }
            else if(dx > 0 && dy > 0)
            {
                objectDirection[i] = DIAG_L2R;
            }
            // TOWARDS direction detection
            //
            // AWAY direction detection
        }
        else
        {
            if(trackedCenter != center)
                objects[i] = center;
        }

        frameUpdated[i] = true;

        circle(img, center, 2, Scalar(0,0,255), 2);
        putText(img, SSTR(i), Point(r.tl().x + 5, r.tl().y + 10), FONT_HERSHEY_SIMPLEX, 0.25, Scalar(50, 170, 50), 1);
    }

    for(int i = 0; i < 16; i++)
    {
        if(!frameUpdated[i])
        {
            objects[i] = Point(0,0);
            objectDirection[i] = NO_STATE;
        }
        else
            frameUpdated[i] = false;

    }
}

void AnalyticalEngine::updateProbabilities()
{
    float sum = 0.0f;
    // Get total decisions made for the last state (state before decision was made)
    for(int j = 0; j < 7; j++)
    {
        sum += decisionMatrix[lastState][j];
    }

    // update the probabilities
    for(int j = 0; j < 7; j++)
    {
        transitionMatrix[lastState][j] = decisionMatrix[lastState][j] / sum;
    }
}

AnalyticalEngine::ActionState AnalyticalEngine::GetMostProbableAction()
{
    uint8_t hiProbIdx = -1, hi = -1, p = 0;
    vector<int> ties;
    ActionState as;

    for(int j = 0; j < 7; j++)
    {
        p = transitionMatrix[presentState][j];
        if(p > hi)
        {
            hiProbIdx = j;
            hi = p;
        }
        else if (p == hi)
        {
            if(ties.size() == 0)
                ties.push_back(hiProbIdx);

            ties.push_back(p);
        }
    }

    if(ties.size() > 0)
    {
        int stateIdx = -1;
        for(int i = 0; i < ties.size(); i++)
        {
            if(ties[i] == 7) { continue; }
        }
        // Tie breaker conditions:
        //  1) State 7 (STOP) always has lowest priority
        //  2) If Reveles is close to a wall (need to determine 'close'),
        //      then actions away from the wall have higher priority.
        //      i.e., if Reveles is near the right wall, and the person is near the right wall,
        //      then we can assume the person will probably not move L2R or L2R_DIAG.
        //      NOTE: Right is relative to Reveles.
        //  3) A person walking L2R, or R2L, will most likely NOT make a 180 deg turn.
        //      i.e. A person walking L2R, won't move R2L to avoid Reveles (at least not without
        //      stopping first).

        // Under most circumstances, action should only be taken if a person is determined
        // to be in front of and moving TOWARDS Reveles, or off to a side and moving diagonally
        // toward Reveles. Meaning a person off the front-left corner is moving L2R_DIAG.
    }

    // May need to add some form of confidence measurement with the
    //  returned data. This confidence measure would affect the speed
    //  Reveles traveles at. i.e. a lower confidence level would cause
    //  Reveles to slow down before making a decision.

    return as;
}

void AnalyticalEngine::Save()
{
    if(!QDir("Assets/Data").exists())
    {
        QDir().mkdir("Assets/Data");
    }

    QDir().cd("Assets/Data");

    QFile tmSaveData("transitionMatrix.csv");
    if(!tmSaveData.open(QIODevice::WriteOnly))
    {
        cout << "Could not open Transition Matrix save file for writing." << endl;
    }
    else
    {
        QByteArray bArr;
        for(int i = 0; i < 7; i++)
        {
            for(int j = 0; j < 7; j++)
            {
                bArr += QString(QString::number(transitionMatrix[i][j]) + ",").toStdString().c_str();
            }
        }

        tmSaveData.write(bArr);
    }

    if(tmSaveData.isOpen())
        tmSaveData.close();

    QFile dmSaveData("decisionMatrix.csv");
    if(!dmSaveData.open(QIODevice::WriteOnly))
    {
        cout << "Could not open Decision Matrix save file for writing" << endl;
    }
    else
    {
        QByteArray bArr;
        for(int i = 0; i < 7; i++)
        {
            for(int j = 0; j < 7; j++)
            {
                bArr += QString(QString::number(decisionMatrix[i][j]) + ",").toStdString().c_str();
            }
        }

        dmSaveData.write(bArr);
    }


    if(dmSaveData.isOpen())
        dmSaveData.close();

}
