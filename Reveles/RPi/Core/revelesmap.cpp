#include "revelesmap.h"

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
                                   offsetLat + (.00005 * (x +1)),
                                   offsetLong + (.00005 * (y +1))
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
