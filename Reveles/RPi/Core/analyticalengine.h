#ifndef REVELES_ANALYTICALENGINE_H
#define REVELES_ANALYTICALENGINE_H

#include "rpi.h"
#include "revelescore.h"

/*
 * The AnalyticalEngine class is intended to process
 * the environment and make adjustments to Reveles' path
 * as needed. This class falls under the NEPU subsystem
 * discussed in the documentation.
 */
class AnalyticalEngine : public QObject
{
    Q_OBJECT

public:
    static AnalyticalEngine *instance();
    AnalyticalEngine();

    void Init();
    void Start(bool demo);


public slots:
    void aboutToQuit();
    void stop();
    void SetMotorDirection(uint8_t dir);

signals:
    void StairsDetected();

private:
    // Variables
    float us[2];
    float tof[8];
    bool pir[2], demoMode;


    // The transition matrix is a 7x7 array
    // of probabilities. The index at (i,j)
    // represents the probability that a
    // person will take a certain action to
    // avoid Reveles. These probabilites are
    // adjusted after every encounter.
    //                   [ps][ns]
    float transitionMatrix[7][7] =
    {
   // ps:  1      2      3      4      5      6      7       ns:
        {0.25f, 0.00f, 0.25f, 0.25f, 0.00f, 0.00f, 0.25f}, // 1
        {0.00f, 0.25f, 0.25f, 0.25f, 0.00f, 0.00f, 0.25f}, // 2
        {0.17f, 0.17f, 0.17f, 0.00f, 0.17f, 0.17f, 0.17f}, // 3
        {0.25f, 0.25f, 0.00f, 0.25f, 0.00f, 0.00f, 0.25f}, // 4
        {0.00f, 0.00f, 0.33f, 0.00f, 0.33f, 0.00f, 0.33f}, // 5
        {0.00f, 0.00f, 0.33f, 0.00f, 0.00f, 0.33f, 0.33f}, // 6
        {0.14f, 0.14f, 0.14f, 0.14f, 0.14f, 0.14f, 0.14f}  // 7
    };

    // This is used to reset the transition matrix
    //                           [ps][ns]
    float transitionMatrix_default[7][7] =
    {
   // ps:  1      2      3      4      5      6      7       ns:
        {0.25f, 0.00f, 0.25f, 0.25f, 0.00f, 0.00f, 0.25f}, // 1
        {0.00f, 0.25f, 0.25f, 0.25f, 0.00f, 0.00f, 0.25f}, // 2
        {0.17f, 0.17f, 0.17f, 0.00f, 0.17f, 0.17f, 0.17f}, // 3
        {0.25f, 0.25f, 0.00f, 0.25f, 0.00f, 0.00f, 0.25f}, // 4
        {0.00f, 0.00f, 0.33f, 0.00f, 0.33f, 0.00f, 0.33f}, // 5
        {0.00f, 0.00f, 0.33f, 0.00f, 0.00f, 0.33f, 0.33f}, // 6
        {0.14f, 0.14f, 0.14f, 0.14f, 0.14f, 0.14f, 0.14f}  // 7
    };

    // The decision matrix is used to track
    // the ACTUAL state transition. i.e. the action
    // a person took to avoid walking into Reveles,
    // NOT the action Reveles expected them to take.
    int decisionMatrix[7][7] =
    {
  // ps: 1  2  3  4  5  6  7     ns:
        {0, 0, 0, 0, 0, 0, 0}, // 1
        {0, 0, 0, 0, 0, 0, 0}, // 2
        {0, 0, 0, 0, 0, 0, 0}, // 3
        {0, 0, 0, 0, 0, 0, 0}, // 4
        {0, 0, 0, 0, 0, 0, 0}, // 5
        {0, 0, 0, 0, 0, 0, 0}, // 6
        {0, 0, 0, 0, 0, 0, 0}  // 7
    };

    int zoneCount[4] = {0, 0, 0, 0};

    ActionState lastState, presentState, predictedState, actualState;
    bool endAnalyze;

    uint8_t motorDir, servoDir;

    QFuture<void> future;

    // Functions
    void CheckEnv();
    void ProcessEnv();

    void AdjustPath_Inanimate();
    void AdjustPath_Animate();

    void updateProbabilities();

    ActionState GetMostProbableAction();

    void Save();

};


#endif //REVELES_ANALYTICALENGINE_H
