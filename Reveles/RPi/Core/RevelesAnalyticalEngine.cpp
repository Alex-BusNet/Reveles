#include "RevelesAnalyticalEngine.h"
#include <QtConcurrent/QtConcurrent>

Q_GLOBAL_STATIC(RevelesAnalyticalEngine, rae)

RevelesAnalyticalEngine *RevelesAnalyticalEngine::instance()
{
    return rae;
}

/*
 * Initialize variables for the analytical engine
 */
void RevelesAnalyticalEngine::Init()
{

}

void RevelesAnalyticalEngine::updateLocation(GPSCoord loc)
{
    currentLocation = loc;
}

/*
 * Begin traversal.
 * @param loc: starting location
 * @param dest: target destination
 */
void RevelesAnalyticalEngine::Start(GPSCoord dest)
{
    destination = dest;
    QFuture<void> pathGen = QtConcurrent::run(this, RevelesAnalyticalEngine::FindPath);
}

/*
 * Empty C'tor needed for Q_GLOBAL_STATIC macro.
 * Place anything in the Init() function that would
 * normally go in the c'tor.
 */

RevelesAnalyticalEngine::RevelesAnalyticalEngine()
{

}

/*
 * FindPath() uses the A* pathfinding algorithm
 */
void RevelesAnalyticalEngine::FindPath()
{
    // Try to find a path to the destination

    // If a path does not exist, then travel to point closest to the destination
    //  then switch to blind navigation mode.
}

QList<GPSCoord> RevelesAnalyticalEngine::GetNeighbors(GPSCoord node)
{

}

void RevelesAnalyticalEngine::DeadReckon()
{

}

int RevelesAnalyticalEngine::GetDistance(GPSCoord a, GPSCoord b)
{

}

/*
 * Navigate is used to travel when we do not know
 * how to fully reach the destination (Blind mode)
 * or have no destination at all (Wander mode)
 */
void RevelesAnalyticalEngine::Navigate()
{
    // Wander mode:
    //      Find node in map data that is adjacent to an UNKNOWN
    //      node type.
    //          If no node is found, return.
    //          If a node is found, go to it. Continue until no
    //           UNKNOWN nodes are left on map.
}

void RevelesAnalyticalEngine::CheckEnv()
{

}

void RevelesAnalyticalEngine::ProcessEnv()
{

}

void RevelesAnalyticalEngine::AdjustPath_Inanimate()
{

}

void RevelesAnalyticalEngine::AdjustPath_Animate()
{

}
