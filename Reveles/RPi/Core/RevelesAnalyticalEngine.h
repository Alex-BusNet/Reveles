#ifndef REVELES_REVELESANALYTICALENGINE_H
#define REVELES_REVELESANALYTICALENGINE_H
#include <QObject>
#include <QList>
#include "Common/datatypes.h"
#include "revelescore.h"

//class RevelesCore;

class RevelesAnalyticalEngine : public QObject
{
    Q_OBJECT

public:
    static RevelesAnalyticalEngine *instance();
    RevelesAnalyticalEngine();

    void Init();
    void Start(GPSCoord dest);

public slots:
    void updateLocation(GPSCoord loc);

private:

    // Functions
    void FindPath();
    QList<GPSCoord> GetNeighbors(GPSCoord node);
    int GetDistance(GPSCoord a, GPSCoord b);

    void Navigate();

    void DeadReckon();
    void CheckEnv();
    void ProcessEnv();

    void AdjustPath_Inanimate();
    void AdjustPath_Animate();

    // Variables
    QList<GPSCoord> path;
    GPSCoord currentLocation, destination;
};


#endif //REVELES_REVELESANALYTICALENGINE_H
