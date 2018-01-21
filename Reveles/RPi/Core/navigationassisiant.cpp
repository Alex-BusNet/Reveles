#include "navigationassisiant.h"
#include <QtConcurrent>
#include <tuple>

#define TI(a,b) (std::tuple<int, int>(a, b))
#define TD(a,b) (std::tuple<double, double>(a,b))
#define TF(a,b) (std::tuple<float, float>(a,b))

Q_GLOBAL_STATIC(NavigationAssisiant, navi)

NavigationAssisiant *NavigationAssisiant::instance()
{
    return navi;
}

NavigationAssisiant::NavigationAssisiant()
{
    /*
     * Empty C'tor needed for Q_GLOBAL_STATIC macro.
     * Place anything in the Init() function that would
     * normally go in the c'tor.
     */
}

void NavigationAssisiant::Init()
{
    destination = GPSCoord{0,0};
    Orient();
}

/*!
 * @brief Begin traversal.
 * @param dest target destination
 */
void NavigationAssisiant::Start(GPSCoord dest)
{
    destination = dest;

    FindPath();
}

void NavigationAssisiant::updateLocation(GPSCoord loc)
{
    currentLocation = loc;
}

/*
 * FindPath() uses the A* pathfinding algorithm
 */
void NavigationAssisiant::FindPath()
{
    // Try to find a path to the destination

    // If a path does not exist, then travel to point closest to the destination
    //  then switch to blind navigation mode.
}

QList<GPSCoord> NavigationAssisiant::GetNeighbors(GPSCoord node)
{

}

int NavigationAssisiant::GetDistance(GPSCoord a, GPSCoord b)
{

}

void NavigationAssisiant::Orient()
{
    MagDirection md = RevelesIO::instance()->ReadMagnetometer();
    Vector2i locVec(currentLocation.latitude, currentLocation.longitude);
    Vector2i destVec(destination.latitude, destination.longitude);

    // relate the gausian values (x, z) to the Direction enumeration.

    if(destVec == TI(0,0))
    {
        // No destination set
    }
    else
    {
        // Determine:
        //  Turn direction
        //  Turn distance
    }
}

/*
 * Navigate is used to travel when we do not know
 * how to fully reach the destination (Blind mode)
 * or have no destination at all (Wander mode)
 */
void NavigationAssisiant::Navigate()
{
    // Wander mode:
    //      Find node in map data that is adjacent to an UNKNOWN
    //      node type.
    //          If no node is found, return.
    //          If a node is found, go to it. Continue until no
    //           UNKNOWN nodes are left on map.
}

void NavigationAssisiant::DeadReckon()
{
    // Need to find the relation between travel speed,
    // and degrees of lat/long changed.
    // This will also need to be adjusted with vectoring as well.
}
