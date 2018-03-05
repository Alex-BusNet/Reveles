#include "analyticalengine.h"
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
    instance()->setObjectName("AnalyticalEngine");

    predictedState = NO_STATE;
    actualState = NO_STATE;
    presentState = NO_STATE;
    lastState = NO_STATE;
    endAnalyze = false;
    tof[0] = tof[1] = tof[2] = tof[3] = tof[4] = tof[5] = tof[6] = tof[7] = 66; // inches
    us = 160; // inches
    pir = false;
    motorDir = M_STOP;
    servoDir = RET_NEUTRAL;

    QFile tmSaveData("Assets/SaveData/transitionMatrix.csv");
    if(!tmSaveData.open(QIODevice::ReadOnly))
    {
        Logger::writeLine(instance(), Reveles::NO_TRANS_MAT);
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

    QFile dmSaveData("Assets/SaveData/decisionMatrix.csv");
    if(!dmSaveData.open(QIODevice::ReadOnly))
    {
        Logger::writeLine(instance(), Reveles::NO_DECISION_MAT);
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
    future = QtConcurrent::run([=]() {
        while(!endAnalyze)
        {
            CheckEnv();
            ProcessEnv();
            delay(500);
        }
    });
}

void AnalyticalEngine::SetMotorDirection(uint8_t dir)
{
    motorDir = dir;
}

void AnalyticalEngine::aboutToQuit()
{
    Logger::writeLine(instance(), QString("Exiting AnalyticalEngine..."));
    this->stop();
    this->Save();
    Logger::writeLine(instance(), QString("Done."));
}

void AnalyticalEngine::stop()
{
    Logger::writeLine(instance(), QString("Ending environment analysis..."));
    endAnalyze = true;

    if(future.isRunning())
        future.cancel();
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
    Logger::writeLine(instance(), QString("CheckEnv()"));
    Logger::writeLine(instance(), QString("Motor: 0x%1").arg(motorDir, 2, HEX));
    Logger::writeLine(instance(), QString("Servo: 0x%1").arg(servoDir, 2, HEX));

    if(motorDir == M_FWD)
    {
        pir = RevelesIO::instance()->readPIR(false);
        us = RevelesIO::instance()->triggerUltrasonic(US_FRONT); // Stair US
        Logger::writeLine(instance(), QString("Front US Reading: %1").arg(us));
        /* overwritten for demo purposes */
        us = 13;
        tof[1] = 65;    // RevelesIO::instance()->ReadTimeOfFlight(1);
    }
    else if(motorDir == M_REV)
    {
        pir = RevelesIO::instance()->readPIR(true);
        us = RevelesIO::instance()->triggerUltrasonic(US_RIGHT); // Stair US
        Logger::writeLine(instance(), QString("Rear US reading: %1").arg(us));
        us = 13;
        //tof[5] = RevelesIO::instance()->ReadTimeOfFlight(5);
        tof[5] = 65;
    }

    if(servoDir == TURN_LEFT)
    {
        if(motorDir == M_FWD)
        {
            tof[1] = RevelesIO::instance()->ReadTimeOfFlight(1);
            tof[0] = RevelesIO::instance()->ReadTimeOfFlight(0);
        }
        else if(motorDir == M_REV)
        {
            tof[5] = RevelesIO::instance()->ReadTimeOfFlight(5);
            tof[6] = RevelesIO::instance()->ReadTimeOfFlight(6);
        }
    }
    else if(servoDir == TURN_RIGHT)
    {
        if(motorDir == M_FWD)
        {
            tof[1] = RevelesIO::instance()->ReadTimeOfFlight(1);
            tof[2] = RevelesIO::instance()->ReadTimeOfFlight(2);
        }
        else if(motorDir == M_REV)
        {
            tof[4] = RevelesIO::instance()->ReadTimeOfFlight(4);
            tof[5] = RevelesIO::instance()->ReadTimeOfFlight(5);
        }
    }

    tof[3] = RevelesIO::instance()->ReadTimeOfFlight(3);
    tof[7] = RevelesIO::instance()->ReadTimeOfFlight(7);
    Logger::writeLine(instance(), QString("PIR: %1").arg(B2STR(pir)));
    Logger::writeLine(instance(), QString("ToF readings:\n"
                                  "             [0]: %1 [1]: %2 [2]: %3\n"
                                  "             [7]: %4         [3]: %5\n"
                                  "             [6]: %6 [5]: %7 [4]: %8")
                      .arg(tof[0], 2, DEC).arg(tof[1], 2, DEC).arg(tof[2], 2, DEC)
                      .arg(tof[7], 2, DEC).arg(tof[3], 2, DEC)
                      .arg(tof[6], 2, DEC).arg(tof[5], 2, DEC).arg(tof[4], 2, DEC));
}

/*!
 * \brief Determine how to avoid obstacle(s)
 */
void AnalyticalEngine::ProcessEnv()
{
    Logger::writeLine(instance(), QString("ProcessEnv()"));
    // DON'T GO DOWN THE STAIRS!!!
    if(us < 12 /* inches */)
    {
        RevelesIO::instance()->EnqueueRequest(RIOData{ IO_MOTOR, M_STOP, 0 });

        if(motorDir == M_FWD)
            motorDir = M_REV;
        else if(motorDir == M_REV)
            motorDir = M_FWD;

        RevelesIO::instance()->EnqueueRequest(RIOData{ IO_MOTOR, motorDir, MOTOR_MAX_SPEED });

        Logger::writeLine(instance(), QString("STAIRS FOUND! Backtracking..."));
//        emit StairsDetected();
        return;
    }
    else
        RevelesIO::instance()->EnqueueRequest(RIOData{ IO_MOTOR, motorDir, us});

    // Simple path adjustment for now. Values are estimates.
    // Person found (distance unknown) or ToF return distance less than max
    if(pir || ((motorDir == M_FWD) && (tof[1] < 66)))
    {
//        RevelesIO::instance()->EnqueueRequest(RIOData{IO_MOTOR, motorDir, MOTOR_MAX_SPEED});
        AdjustPath_Inanimate();
    }
    else if(pir || (motorDir == M_REV) && (tof[5] < 66))
    {

//        RevelesIO::instance()->EnqueueRequest(RIOData{IO_MOTOR, motorDir, MOTOR_MAX_SPEED});
        AdjustPath_Inanimate();
    }

    // if we get a signal from the PIR and the corresponding ToF 
    // sensor reads more than E-Stop distance, then we know we still
    // have space to move out of the way of person.

#ifdef USE_OBJ_DETECT
    // Check the ObjectDetector for any objects.
    if(!DetectionQueue::isEmpty())
    {
        ObjectTracking ot = DetectionQueue::dequeue();
        if(ot.dir != NO_STATE)
            zoneCount[ot.zone]++;
        else
            zoneCount[ot.zone]--;

        cout << "{NL}[ AnalyticalEngine ] Dir: " << ot.dir << " Index: " << ot.index << " Zone: " << ot.zone << endl;
    }
#endif

    /// TODO: Multi obstacle avoidance handling.
    ///     Note: Some of this handling will
    ///             be able to use available map data
    ///             to determine if a short response distance
    ///             is a foreign object or a wall.
}

void AnalyticalEngine::AdjustPath_Inanimate()
{
    Logger::writeLine(instance(), QString("AdjustPath_Inanimate()"));
//    RevelesIO::instance()->EnqueueRequest(RIOData {IO_MOTOR, motorDir, MOTOR_HALF_SPEED});
    RevelesIO::instance()->EnqueueRequest(RIOData{IO_MOTOR, motorDir, MOTOR_MAX_SPEED});

    // Read ToF for right side
    if (tof[3] > 36) // inches
    {
        // Timing between commands may need to be adjusted.
        RevelesIO::instance()->EnqueueRequest(RIOData{ IO_SERVO, TURN_RIGHT, 45 });
        servoDir = TURN_RIGHT;
        delay(1000);
        RevelesIO::instance()->EnqueueRequest(RIOData{ IO_SERVO, RET_NEUTRAL, 0 });
        servoDir = RET_NEUTRAL;
        delay(1000);
        RevelesIO::instance()->EnqueueRequest(RIOData{ IO_SERVO, TURN_LEFT, 45 });
        servoDir = TURN_LEFT;
        delay(1000);
        RevelesIO::instance()->EnqueueRequest(RIOData{ IO_SERVO, RET_NEUTRAL, 0 });
        servoDir = RET_NEUTRAL;
    }
    else
    {
        // Read ToF for left side
        if (tof[7] > 36) // inches
        {
            // Timing between commands may need to be adjusted
            RevelesIO::instance()->EnqueueRequest(RIOData{ IO_SERVO, TURN_LEFT, 45 });
            servoDir = TURN_LEFT;
            delay(1000);
            RevelesIO::instance()->EnqueueRequest(RIOData{ IO_SERVO, RET_NEUTRAL, 0 });
            servoDir = RET_NEUTRAL;
            delay(1000);
            RevelesIO::instance()->EnqueueRequest(RIOData{ IO_SERVO, TURN_RIGHT, 45 });
            servoDir = TURN_RIGHT;
            delay(1000);
            RevelesIO::instance()->EnqueueRequest(RIOData{ IO_SERVO, RET_NEUTRAL, 0 });
            servoDir = RET_NEUTRAL;
        }
    }
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
    if(!QDir("Assets/SaveData").exists())
    {
        QDir().mkdir("Assets/SaveData");
    }

    QFile tmSaveData("Assets/SaveData/transitionMatrix.csv");
    if(!tmSaveData.open(QIODevice::WriteOnly))
    {
        Logger::writeLine(instance(), Reveles::TRANS_MAT_SAVE_ERR);
    }
    else
    {
        Logger::writeLine(instance(), Reveles::TRANS_MAT_SAVE_PROG);
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

    QFile dmSaveData("Assets/SaveData/decisionMatrix.csv");
    if(!dmSaveData.open(QIODevice::WriteOnly))
    {
        Logger::writeLine(instance(), Reveles::DECISION_MAT_SAVE_ERR);
    }
    else
    {
        Logger::writeLine(instance(), Reveles::DECISION_MAT_SAVE_PROG);
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

    Logger::writeLine(instance(), "Save complete.");

}
