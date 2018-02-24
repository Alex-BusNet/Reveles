#ifndef NAVIGATIONASSISIANT_H
#define NAVIGATIONASSISIANT_H

#include "rpi.h"

#include <QSet>
#include <Common/vector2f.h>
#include <Common/vectorpolar2f.h>

#include "revelesmap.h"
#include "revelescore.h"

/*
 * The NavigationAssistant class is intended to
 * handle the more traditional traversal tasks
 * such as pathfinding and updating map nodes.
 */
class NavigationAssisiant : public QObject
{
    Q_OBJECT

public:
    NavigationAssisiant();
    static NavigationAssisiant *instance();

    void Init();
    void Start(GPSCoord dest);
    void Orient();
    void FindBearing();

    double GetDistance(GPSCoord pt1, GPSCoord pt2);

public slots:
    void updateLocation(GPSCoord loc);
    void EStop();

signals:
    void PathReady(QVector<GPSCoord> path);
    void PathUpdate();

private:
    // Functions
    void FindPath();
    QList<MapNode*> GetNeighbors(MapNode *node);
    void RetracePath(MapNode *end);

    void Navigate();
    bool VerifyLocation();
    void DeadReckon();

    // Variables
//    QVector<GPSCoord> path;
    MapNode *path;
    MapNode *nextNode;
    MapNode *currentNode;
    GPSCoord currentLocation, destination;
    Vector2f destVec, locVec;
    Direction heading;
    QFuture<void> future;
    double bearingAngle;
    double headingAngle;
};

#endif // NAVIGATIONASSISIANT_H
