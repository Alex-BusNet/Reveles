#ifndef REVELES_ANALYTICALENGINE_H
#define REVELES_ANALYTICALENGINE_H
#include <QObject>
#include <QList>
#include "Common/datatypes.h"
#include "revelescore.h"

// Passive Infrared Select bits
#define PIR_FL 0b000 // Front-left
#define PIR_FR 0b001 // Front-right
#define PIR_BL 0b010 // Back-left
#define PIR_BR 0b011 // Back-right

// Ultrasonic Select bits
#define US_LEFT  0b000
#define US_FRONT 0b001
#define US_RIGHT 0b010
#define US_STAIR 0b011
#define US_BACK  0b100

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

public slots:
    void aboutToQuit();

private:
    // Variables

    int pirs[4], uss[4], tofs[6];
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

    // ActionState represents the detected
    // and predicted actions an moving object
    // would take.
    enum ActionState
    {
        L2R      = 0,// State 1
        R2L      = 1,// State 2
        TOWARDS  = 2,// State 3
        AWAY     = 3,// State 4
        DIAG_L2R = 4,// State 5
        DIAG_R2L = 5,// State 6
        STOP     = 6,// State 7
        NO_STATE = 7 // Initialization State.
    };

    ActionState lastState, presentState, predictedState, actualState;

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