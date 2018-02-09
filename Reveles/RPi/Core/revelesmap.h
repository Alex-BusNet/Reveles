#ifndef REVELESMAP_H
#define REVELESMAP_H

#include <QObject>
#include "Common/datatypes.h"
#include <vector>
#include <list>
#include <string>
#include <map>

using namespace std;

class RevelesMap : public QObject
{
    Q_OBJECT

public:
    RevelesMap();
    static RevelesMap* instance();

    void Init();
    GPSCoord GetOffset();

    void AddPoint(GPSCoord gpsc);
    list<GPSCoord> GetNearbyPoints(GPSCoord gpsc);
    void RegisterDestination(string name, GPSCoord gpsc);
//    list<Node*> FindPath(Node* end);
//    list<Node*> FindPath(string dest);

private:
//    map<string, Node> savedDestinations;
    vector<Node> grid;
    double offsetLat, offsetLong;

    void saveMapData();
    void LoadMapData();
    bool gridHasPoint(GPSCoord gpsc);

    double GetDistance(GPSCoord pt1, GPSCoord pt2);

};

#endif // REVELESMAP_H
