#include "analyticalengine.h"
#include <QtConcurrent/QtConcurrent>
#include <iostream>
#include <QByteArray>
#include <QString>
#include <QStringList>
#include <vector>
#include "Common/detectionqueue.h"


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
    endAnalyze = false;

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

//    for (int i = 0; i < 16; i++)
//    {
//        objects[i] = Point(0,0);
//        frameUpdated[i] = false;
//        objectDirection[i] = NO_STATE;
//    }

}

void AnalyticalEngine::Start()
{
    while(!endAnalyze)
    {
        CheckEnv();
        ProcessEnv();
    }
}

void AnalyticalEngine::aboutToQuit()
{
    this->Save();
}

void AnalyticalEngine::stop()
{
    endAnalyze = true;
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

    // Check the ObjectDetector for any objects.
    if(!DetectionQueue::isEmpty())
    {
        ObjectTracking ot = DetectionQueue::dequeue();
        if(ot.dir != NO_STATE)
            zoneCount[ot.zone]++;
        else
            zoneCount[ot.zone]--;

        cout << ot.dir << " " << ot.index << " " << ot.zone << endl;
    }

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

ActionState AnalyticalEngine::GetMostProbableAction()
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

/*!
 * \brief Save the Markov Chain to file
 * Saves the transition matrix and decision
 * matrix to file.
 */
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
        cout << "Saving Transition Matix" << endl;
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
        cout << "Saving Decision Matrix" << endl;
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
