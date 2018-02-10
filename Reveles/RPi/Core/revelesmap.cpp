#include "revelesmap.h"

#define THRESHOLD_LATITUDE  (0.0000103651 / 2)
#define THRESHOLD_LONGITUDE (0.0000038167 / 2)

Q_GLOBAL_STATIC(RevelesMap, rm)

RevelesMap::RevelesMap()
{

}

RevelesMap *RevelesMap::instance()
{
    return rm;
}

void RevelesMap::Init()
{
    instance()->setObjectName("RevelesMap");
    offsetLat = 41.631906;
    offsetLong = -85.006118;

    // Grid size: 120 tile x 63 tile (approx)
    for(int x = 0; x < 120; x++)
    {
        for(int y = 0; y < 63; y++)
        {
            grid.push_back(Node
                           {
                               GPSCoord
                               {
                                   offsetLat +  (THRESHOLD_LATITUDE * (x +1)),
                                   offsetLong + (THRESHOLD_LONGITUDE * (y +1))
                               },
                               UNKNOWN
                           });
        }
    }
}

GPSCoord RevelesMap::GetOffset()
{
    return GPSCoord{offsetLat, offsetLong};
}

double RevelesMap::GetLatitudeThreshold()
{
    return THRESHOLD_LATITUDE;
}

double RevelesMap::GetLongitudeThreshold()
{
    return THRESHOLD_LONGITUDE;
}

/**
 * @brief Updates the NodeType of the selected node
 * @param x The x coordinate of the tile
 * @param y The y corrdinate of the tile
 * @param t The new NodeType of the tile
 */
void RevelesMap::UpdatePath(int x, int y, NodeType t)
{
    grid.at((x / 2) + (120 * y)).nt = t;
}

void RevelesMap::AddPoint(GPSCoord gpsc)
{
    // One grid square corresponds to a 2ft x 2ft area.
    if(gpsc.latitude < offsetLat)
    {

    }

//    if(!gridHasPoint(gpsc))
//        grid.push_back(gpsc);
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

    return neighbors;
}

void RevelesMap::RegisterDestination(string name, GPSCoord gpsc)
{

}

//list<Node*> RevelesMap::FindPath(Node* end)
//{
//    list<Node*> path;

//    //-------------------
//    // Path Finding Algorithm goes here
//    //-------------------

//    //-------------------
//    /* Retrace Path */

//    //-------------------

//    return path;
//}

///*
// * FindPaths(string) assumes that the destination
// * passed to it exists.
// */
//list<Node*> RevelesMap::FindPath(string dest)
//{
//    return FindPath(&savedDestinations[dest]);
//}

void RevelesMap::saveMapData()
{

}

void RevelesMap::LoadMapData()
{

}

bool RevelesMap::gridHasPoint(GPSCoord gpsc)
{
//    for(GPSCoord c : grid)
//    {
//        if(gpsc == c)
//            return true;
//    }

    return false;
}
