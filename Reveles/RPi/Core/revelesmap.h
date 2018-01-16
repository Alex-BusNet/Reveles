#ifndef REVELESMAP_H
#define REVELESMAP_H

#include "../Common/datatypes.h"
#include <vector>
#include <list>
#include <string>
#include <map>

using namespace std;

class RevelesMap
{
public:
    RevelesMap();
    void AddPoint(GPSCoord gpsc);
    list<GPSCoord> GetNearbyPoints(GPSCoord gpsc);
    void RegisterDestination(string name, GPSCoord gpsc);
    list<Node*> FindPath(Node* end);
    list<Node*> FindPath(string dest);

private:
    map<string, GPSCoord> savedDestinations;
    vector<GPSCoord> grid;
    double offsetLat, offsetLong;

    void saveMapData();
    void LoadMapData();
    bool gridHasPoint(GPSCoord gpsc);

};

#endif // REVELESMAP_H
