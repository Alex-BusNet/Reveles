#include "navigationassisiant.h"
#include <QtConcurrent>
#include <tuple>

#define TUPI(a,b) (std::tuple<int, int>(a, b))
#define TUPD(a,b) (std::tuple<double, double>(a,b))
#define TUPF(a,b) (std::tuple<float, float>(a,b))

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

double NavigationAssisiant::GetDistance(GPSCoord pt1, GPSCoord pt2)
{
    // Calculation from https://www.movable-type.com.uk/scripts/lotlong.html
    // Formula requires Lat/Long be given in decimal form.
    // Note: negative numbers indicate south/west

    double lat1 = ((static_cast<double>(rand()) / RAND_MAX) * 180.0); // pt1.latitude;
    double lon1 = ((static_cast<double>(rand()) / RAND_MAX) * 180.0); // pt1.longitude;

    double lat2 = ((static_cast<double>(rand()) / RAND_MAX) * 180.0); // pt2.latitude;
    double lon2 = ((static_cast<double>(rand()) / RAND_MAX) * 180.0); // pt2.longitude;

    // Calculate distance from Lat/Long using Haversine formula
    int R = 6371e3; // Earth's radius (in meters)

    double lat1_angle = lat1 * (M_PI / 180.0);
    double lat2_angle = lat2 * (M_PI / 180.0);

    double latDelta = (lat2 - lat1) * (M_PI - 180.0);
    double lonDelta = (lon2 - lon1) * (M_PI - 180.0);

    double a = sin(latDelta / 2) * sin(latDelta / 2) +
            cos(lat1_angle) * cos(lat2_angle) *
            sin(lonDelta / 2) * sin(lonDelta / 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return R * c; // Distance travelled between two points.
}

void NavigationAssisiant::Orient()
{
    MagDirection md = RevelesIO::instance()->ReadMagnetometer();
    Vector2f locVec(currentLocation.latitude, currentLocation.longitude);
    Vector2f destVec(destination.latitude, destination.longitude);

    // relate the gausian values (x, z) to the Direction enumeration.
    if(md.x != 0.0)
    {
        // Convert Gaussian values to degrees:
        double deg = std::atan(md.y / md.x) * (180.0 / M_PI);
        if(deg > 360.0)
            deg -= 360.0;
        else if(deg < 0)
            deg += 360.0;

        // determine heading
        if((deg >= 345) || (deg < 15))       { heading = N; }
        else if((deg >= 15) && (deg < 45))   { heading = EN; }
        else if((deg >= 45) && (deg < 75))   { heading = NE; }
        else if((deg >= 75) && (deg < 105))  { heading = E; }
        else if((deg >= 105) && (deg <= 135)){ heading = SE; }
        else if((deg > 135) && (deg < 165))  { heading = ES; }
        else if((deg >= 165) && (deg < 195)) { heading = S; }
        else if((deg >= 195) && (deg < 225)) { heading = WS; }
        else if((deg >= 225) && (deg < 255)) { heading = SW; }
        else if((deg >= 255) && (deg < 285)) { heading = W; }
        else if((deg >= 285) && (deg <= 315)){ heading = NW; }
        else if((deg > 315) && (deg < 345))  { heading = WN; }

        headingAngle = deg;
    }

//    if(destVec != TUPD(0.0, 0.0))
//    {
//        VectorPolar2f resultant = (destVec - locVec).toPolar();

//        // Adjust vector angle to compass angle
//        float rAng = resultant.degrees() + 90;

//        if(rAng < headingAngle)
//        {
//            // Turn right
//        }
//        else
//        {
//            // Turn left
//        }
//    }
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
