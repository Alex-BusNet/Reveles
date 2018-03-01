#ifndef REVELESMAP_H
#define REVELESMAP_H

#include "rpi.h"

#include <vector>
#include <list>
#include <string>
#include <map>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

using namespace std;

class RevelesMap : public QObject
{
    Q_OBJECT

public:
    RevelesMap();
    static RevelesMap* instance();

    void Init();
    GPSCoord GetOffset();
    double GetLatitudeThreshold();
    double GetLongitudeThreshold();
    MapNode *GetNodeFromCoord(const GPSCoord &gpsc);
    MapNode* GetNodeFromPoint(int x, int y);

    void UpdatePath(int x, int y, NodeType t);

    void AddPoint(GPSCoord gpsc);
    list<GPSCoord> GetNearbyPoints(GPSCoord gpsc);
    void RegisterDestination(string name, GPSCoord gpsc);
//    list<Node*> FindPath(Node* end);
//    list<Node*> FindPath(string dest);

private:
//    map<string, Node> savedDestinations;
    QVector<MapNode*> grid;
    double offsetLat, offsetLong;

    void saveMapData();
    void LoadMapData();
    bool gridHasPoint(GPSCoord gpsc);

    double GetDistance(GPSCoord pt1, GPSCoord pt2);

};

#endif // REVELESMAP_H
