#ifndef NAVIGATIONASSISIANT_H
#define NAVIGATIONASSISIANT_H

#include <QObject>
#include <QList>
#include <Common/datatypes.h>
#include <Common/vector2i.h>
#include "revelescore.h"

/*
 * The NavigationAssistant class is intended to
 * handle the more traditional traversal tasks
 * such as pathfinding and updating map nodes.
 */
class NavigationAssisiant
{
public:
    NavigationAssisiant();
    static NavigationAssisiant *instance();

    void Init();
    void Start(GPSCoord dest);

public slots:
    void updateLocation(GPSCoord loc);

private:

    // Functions
    void FindPath();
    QList<GPSCoord> GetNeighbors(GPSCoord node);
    int GetDistance(GPSCoord a, GPSCoord b);

    void Orient();
    void Navigate();

    void DeadReckon();

    // Variables
    QList<GPSCoord> path;
    GPSCoord currentLocation, destination;
    Direction heading;
};

#endif // NAVIGATIONASSISIANT_H
