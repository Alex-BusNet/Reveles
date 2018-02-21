#ifndef NAVIGATIONASSISIANT_H
#define NAVIGATIONASSISIANT_H

#include <Common/datatypes.h>
#include <QObject>
#include <QList>
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

signals:
    void PathReady(QVector<GPSCoord> path);
    void PathUpdate();

private:
    // Functions
    void FindPath();
    QList<MapNode*> GetNeighbors(MapNode *node);
    void RetracePath(MapNode *end);

    void Navigate();

    void DeadReckon();

    // Variables
//    QVector<GPSCoord> path;
    MapNode *path;
    MapNode *nextNode;
    MapNode *currentNode;
    GPSCoord currentLocation, destination;
    Direction heading;
    double bearingAngle;
    double headingAngle;
};

#endif // NAVIGATIONASSISIANT_H
