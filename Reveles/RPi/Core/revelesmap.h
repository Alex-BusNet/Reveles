#ifndef REVELESMAP_H
#define REVELESMAP_H

#include "datatypes.h"
#include <vector>
#include <List>
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
    list<GPSCoord> FindPath(GPSCoord end);
    list<GPSCoord> FindPath(string dest);

private:
    map<string, GPSCoord> savedDestinations;
    vector<GPSCoord> grid;
    void saveMapData();
    void LoadMapData();
    bool gridHasPoint(GPSCoord gpsc);

};

#endif // REVELESMAP_H
