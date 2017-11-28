#include "revelesmap.h"


RevelesMap::RevelesMap()
{

}

void RevelesMap::AddPoint(GPSCoord gpsc)
{
    if(!gridHasPoint(gpsc))
        grid.push_back(gpsc);
}

list<GPSCoord> RevelesMap::GetNearbyPoints(GPSCoord gpsc)
{
    list<GPSCoord> neighbors;

    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            if(x == y) { continue; }

            int checkX;
            int checkY;
        }
    }
}

void RevelesMap::RegisterDestination(string name, GPSCoord gpsc)
{

}

list<GPSCoord> RevelesMap::FindPath(GPSCoord end)
{

}

/*
 * FindPaths(string) assumes that the destination
 * passed to it exists.
 */
list<GPSCoord> RevelesMap::FindPath(string dest)
{
    return FindPath(savedDestinations[dest]);
}

void RevelesMap::saveMapData()
{

}

void RevelesMap::LoadMapData()
{

}

bool RevelesMap::gridHasPoint(GPSCoord gpsc)
{
    for(GPSCoord c : grid)
    {
        if(gpsc == c)
            return true;
    }

    return false;
}
