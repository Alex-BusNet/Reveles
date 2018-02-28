#include "navigationassisiant.h"
#include <tuple>
#include "Common/vectorutils.h"

//#define TUPI(a,b) (std::tuple<int, int>(a, b))
//#define TUPD(a,b) (std::tuple<double, double>(a,b))
//#define TUPF(a,b) (std::tuple<float, float>(a,b))

#define FEET_PER_METER 3.280839895013   // ft/m
#define METER_PER_FOOT 1/3.280839895013 // m/ft

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
    instance()->setObjectName("NavigationAssistant");
    destination = GPSCoord{0,0};
    currentLocation = GPSCoord{41.632559, -85.005982};
    path = NULL;
    Orient();
}

/*!
 * @brief Begin traversal.
 * @param dest target destination
 */
void NavigationAssisiant::Start(GPSCoord dest)
{
    destination = dest;
    locVec.set(currentLocation);
    destVec.set(dest);

    Orient();

    Logger::writeLine(instance(), Reveles::SET_TARGET_DEST.arg(dest.latitude).arg(dest.longitude));
    Logger::writeLine(instance(), QString("Straight-line distance to destination: ") +
                      QString::number(GetDistance(currentLocation, destination)) + " ft");
    MapNode *mn = RevelesMap::instance()->GetNodeFromCoord(currentLocation);
    QString nodeStr = QString("%1, %2").arg(mn->mapX).arg(mn->mapY);
    Logger::writeLine(instance(), QString("CurrentNode: %1").arg(nodeStr));

    mn = RevelesMap::instance()->GetNodeFromCoord(dest);
    nodeStr = QString("%1, %2").arg(mn->mapX).arg(mn->mapY);
    Logger::writeLine(instance(), QString("Dest node: %1").arg(nodeStr));

    Logger::writeLine(instance(), "Finding Path...");
    FindPath();

    Logger::writeLine(instance(), "Starting Navigation");
//    AnalyticalEngine::instance()->Start();
    future = QtConcurrent::run([=]() { Navigate(); });
}

void NavigationAssisiant::updateLocation(GPSCoord loc)
{
    currentLocation = loc;
}

/**
 * @brief NavigationAssisiant::EStop Stops all navigation and restarts.
 * Used when stairs are detected. This will update the map data clear the current path, abort the
 * navigation thread, and restart the navigation process.
 *
 */
void NavigationAssisiant::EStop()
{
    RevelesMap::instance()->UpdatePath(path->child->mapX, path->child->mapY, STAIRS);
    RevelesIO::instance()->EnqueueRequest(RIOData{IO_MOTOR, M_STOP, 0});
    path = NULL;

    if(future.isRunning())
        future.cancel();

    AnalyticalEngine::instance()->stop();

    Start(destination);
}

/*
 * FindPath() uses the A* pathfinding algorithm
 */
void NavigationAssisiant::FindPath()
{
    if(destination == TUPD(0.0, 0.0))
    {
        Logger::writeLine(instance(), Reveles::INVALID_DESTINATION);
        return;
    }
    MapNode *endNode = RevelesMap::instance()->GetNodeFromCoord(destination);
    currentNode = RevelesMap::instance()->GetNodeFromCoord(currentLocation);

    if(endNode == currentNode)
    {
        Logger::writeLine(instance(), Reveles::START_EQUAL_DEST);
        return;
    }

    QList<MapNode*> openSet;
    QSet<MapNode*> closedSet;
    MapNode *mn;

    openSet.push_back(RevelesMap::instance()->GetNodeFromCoord(currentLocation));

    while(openSet.count() > 0)
    {
        mn = openSet[0];

        // Compare the fCost of the current node to the fCost of
        // the node at index i of the openSet. CurrentNode is always
        // the tile at index 0 of the openSet.
        for(int i = 1; i < openSet.count(); i++)
        {
            if((openSet[i]->fCost() < mn->fCost()) || (openSet[i]->fCost() == mn->fCost()))
            {
                mn = openSet[i];
            }
        }

        // Remove the node from the openSet
        // and add it to the closed set
        openSet.removeOne(mn);
        closedSet.insert(mn);

        // If we have reached our destination,
        // quit searching and retrace the path
        // back to the start.
        if(mn == endNode)
        {
            RetracePath(endNode);
            return;
        }

        QList<MapNode*> neighborsList = GetNeighbors(mn);

        foreach(MapNode *n, neighborsList)
        {
            // Check if the tile is non-traversable
            if((n->nt == WALL) || (n->nt == STAIRS) || closedSet.contains(n))
               continue;

            int newMoveCostToNeighbor = mn->gCost + GetDistance(mn->coord, n->coord);

            if((newMoveCostToNeighbor < n->gCost) || (!openSet.contains(n)))
            {
                n->gCost = newMoveCostToNeighbor;
                n->hCost = GetDistance(n->coord, endNode->coord);
                n->parent = mn;

                if(!openSet.contains(n))
                {
                    openSet.push_back(n);
                }

            }
        }
    }
}

QList<MapNode *> NavigationAssisiant::GetNeighbors(MapNode *node)
{
    QList<MapNode*> neighbors;

    for(int x = -1; x <= 1; x++)
    {
        for(int y = -1; y <= 1; y++)
        {
            if((x == 0) && (y == 0))
                continue;

            int checkX = node->mapX + x;
            int checkY = node->mapY + y;

            if((checkX >= 0) && (checkX < 120) && (checkY >= 0) && (checkY < 63))
                neighbors.push_back(RevelesMap::instance()->GetNodeFromPoint(checkX, checkY));
        }
    }

    return neighbors;
}

void NavigationAssisiant::RetracePath(MapNode *end)
{
    Logger::writeLine(instance(), QString("RetracePath()"));
    MapNode *current = end;
    QList<MapNode*> temp;

    do
    {
        temp.push_back(current);
        current = current->parent;
    }
    while(current != currentNode);

    int i = 0, j = temp.size()/* - 1*/;

    while(i < j)
    {
        current->parent->child = current;
        current = current->parent;
//        MapNode *n = temp[i];
//        temp[i] = temp[j];
//        temp[j] = n;
        i++;
//        j--;
    }

    // This should be a linked list from
    // the currentLocation to the destination.
    path = current;
    Logger::writeLine(instance(), QString("Done"));
}

/**
 * @brief Calculates the distance between two GPSCoord objects
 * @param pt1
 * @param pt2
 * @return The distance between two points in feet.
 */
double NavigationAssisiant::GetDistance(GPSCoord pt1, GPSCoord pt2)
{
    // Calculation from https://www.movable-type.com.uk/scripts/lotlong.html
    // Formula requires Lat/Long be given in decimal form.
    // Note: negative numbers indicate south/west

#ifdef DEBUG_NAV
    double lat1 = ((static_cast<double>(rand()) / RAND_MAX) * 180.0); // pt1.latitude;
    double lon1 = ((static_cast<double>(rand()) / RAND_MAX) * 180.0); // pt1.longitude;

    double lat2 = ((static_cast<double>(rand()) / RAND_MAX) * 180.0); // pt2.latitude;
    double lon2 = ((static_cast<double>(rand()) / RAND_MAX) * 180.0); // pt2.longitude;
#else
    double lat1 = pt1.latitude;
    double lat2 = pt2.latitude;
    double lon1 = pt1.longitude;
    double lon2 = pt2.longitude;
#endif

    // Calculate distance from Lat/Long using Haversine formula
    int R = 6371e3; // Earth's radius (in meters)

    double lat1_angle = lat1 * (M_PI / 180.0);
    double lat2_angle = lat2 * (M_PI / 180.0);

    double latDelta = (lat2 - lat1) * (M_PI / 180.0);
    double lonDelta = (lon2 - lon1) * (M_PI / 180.0);

    double a = sin(latDelta / 2) * sin(latDelta / 2) +
            cos(lat1_angle) * cos(lat2_angle) *
            sin(lonDelta / 2) * sin(lonDelta / 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    // Distance travelled between two points (in feet)
    return ((R * c) * FEET_PER_METER);
}

void NavigationAssisiant::Orient()
{
    MagDirection md = RevelesIO::instance()->ReadMagnetometer();
    double deg;

    // relate the gausian values (x, y) to the Direction enumeration.
    if(md.x != 0.0)
    {
        // Convert Gaussian values to degrees:
        deg = std::atan(md.y / md.x) * (180.0 / M_PI);

        // The values from atan() are fixes between -90 and 90,
        // So we will multiply them by 2 to fix the angle between
        // -180 and 180
        deg *= 2.0;
    }
    else if(md.y < 0.0)
    {
        deg = 90.0;
    }
    else
    {
        deg = 0.0;
    }

    // determine heading
    if((deg >= -15) && (deg < 15))         { heading = N;  }
    else if((deg >= 15) && (deg < 45))     { heading = EN; }
    else if((deg >= 45) && (deg < 75))     { heading = NE; }
    else if((deg >= 75) && (deg < 105))    { heading = E;  }
    else if((deg >= 105) && (deg <= 135))  { heading = SE; }
    else if((deg > 135) && (deg < 165))    { heading = ES; }
    else if((deg >= 165) || (deg < -165))  { heading = S;  }
    else if((deg >= -165) && (deg < -135)) { heading = WS; }
    else if((deg >= -135) && (deg < -105)) { heading = SW; }
    else if((deg >= -105) && (deg < -75))  { heading = W;  }
    else if((deg >= -75) && (deg <= -45))  { heading = NW; }
    else if((deg > -45) && (deg < -15))    { heading = WN; }

    headingAngle = deg;
    Logger::writeLine(instance(), QString("Heading: %1").arg(QString(DirString[heading].c_str())));
}

void NavigationAssisiant::FindBearing()
{
    if(currentNode != NULL && nextNode != NULL)
    {
        double phi1 = locVec.x() * (M_PI / 180.0);
        double phi2 = destVec.x() * (M_PI / 180.0);
        double lambda1 = locVec.y() * (M_PI / 180.0);
        double lambda2 = destVec.y() * (M_PI / 180.0);

        double y = sin(lambda2 - lambda1) * cos(phi2);
        double x = cos(phi1) * sin(phi2) - sin(phi1) * cos(phi2) * cos(lambda2 - lambda1);

        // Bearing (in degrees)
        double b = atan2(y, x) * (180.0 / M_PI);

        // Convert bearing to compass bearing
        double cb = fmod(b + 360.0,  360.0);

        //Final bearing
        double fb = fmod(b + 180.0, 360.0);

        Logger::writeLine(instance(), QString("        Bearing: %1 deg.").arg(b));
        Logger::writeLine(instance(), QString("Compass Bearing: %1 deg.").arg(cb));
        Logger::writeLine(instance(), QString("  Final Bearing: %1 deg.").arg(fb));

        bearingAngle = b;
    }
}

void NavigationAssisiant::End()
{
    if(future.isRunning())
        future.cancel();

    path = NULL;

    RevelesIO::instance()->EnqueueRequest(RIOData{IO_MOTOR, M_STOP, 0});
}

/*
 * Navigate is used to travel when we do not know
 * how to fully reach the destination (Blind mode)
 * or have no destination at all (Wander mode)
 */
void NavigationAssisiant::Navigate()
{
    while(path != NULL)
    {
        currentNode = path;
        nextNode = path->child;

        /// TODO: Need to test how this behaves with the AnalyticalEngine adjusting the path.
        if(!VerifyLocation())
        {
            Logger::writeLine(instance(), Reveles::COORD_MISMATCH);
            RevelesIO::instance()->EnqueueRequest(RIOData{IO_MOTOR, M_STOP, 0});
            // Reveles is not moving at the same pace that the path is updating.
            // Therefore, Reveles should stop and recalculate its path.
            FindPath();
            continue;
        }

        if(currentNode == NULL)
        {
            Logger::writeLine(instance(), Reveles::END_OF_PATH);
            return;
        }

        if(currentNode->nt == UNKNOWN)
        {
            int tofDist = RevelesIO::instance()->ReadTimeOfFlight(1);
            if(tofDist < 24 && !RevelesIO::instance()->readPIR(FRONT_US_PIR))
            {
                RevelesIO::instance()->EnqueueRequest(RIOData{IO_MOTOR, M_STOP, 0});
                RevelesMap::instance()->UpdatePath(currentNode->mapX, currentNode->mapY, WALL);
                FindPath();
                // We are telling the loop to start over
                // because we have changed the path.
                continue;
            }
            else
            {
                RevelesMap::instance()->UpdatePath(currentNode->mapX, currentNode->mapY, PATH);
            }
        }

        FindBearing();

        locVec.set(currentNode->coord);

        // if angle is not about 0 degrees,
        // then Reveles needs to turn.
        //
        //  angle < 180 => turn right
        //  angle > 180 => turn left
        //
        // Remember to back up slowly! Don't floor it!

        if(destVec != TUPF(0.0, 0.0))
        {
            VectorPolar2f resultant = toPolar(destVec - locVec);

            // Adjust vector angle to compass angle
            float rAng = 90 - resultant.degrees();
            Logger::writeLine(instance(), QString("rAng: %1\t headingAngle: %2").arg(rAng, 5, 'g', 10).arg(headingAngle, 5, 'g', 10));
            if(rAng < -5 && rAng > 5)
            {
                if(rAng < headingAngle)
                {
                    // Turn right
                    if(rAng > 90)
                        RevelesIO::instance()->EnqueueRequest(RIOData{IO_SERVO, TURN_RIGHT, 90});
                    else
                        RevelesIO::instance()->EnqueueRequest(RIOData{IO_SERVO, TURN_RIGHT, rAng});
                }
                else
                {
                    // Turn left
                    if(rAng < -90)
                        RevelesIO::instance()->EnqueueRequest(RIOData{IO_SERVO, TURN_LEFT, -90});
                    else
                        RevelesIO::instance()->EnqueueRequest(RIOData{IO_SERVO, TURN_LEFT, rAng});
                }

                if((rAng < -90 && rAng >= -180) || (rAng > 90 && rAng <= 180))
                    RevelesIO::instance()->EnqueueRequest(RIOData{IO_MOTOR, M_REV, MOTOR_HALF_SPEED});
                else if((rAng > -90 && rAng <= 180))
                    RevelesIO::instance()->EnqueueRequest(RIOData{IO_MOTOR, M_FWD, MOTOR_MAX_SPEED});
            }
            else
            {
                RevelesIO::instance()->EnqueueRequest(RIOData{IO_SERVO, RET_NEUTRAL, 0});
                RevelesIO::instance()->EnqueueRequest(RIOData{IO_SERVO, M_FWD, MOTOR_MAX_SPEED});
            }
        }

        delay(500);
    }

    Logger::writeLine(instance(), Reveles::END_OF_PATH);
    AnalyticalEngine::instance()->stop();
    RevelesIO::instance()->StopNav();

    // Wander mode:
    //      Find node in map data that is adjacent to an UNKNOWN
    //      MapNode type.
    //          If no node is found, return.
    //          If a node is found, go to it. Continue until no
    //           UNKNOWN nodes are left on map.
}

bool NavigationAssisiant::VerifyLocation()
{
//    RevelesIO::instance()->SendGPSRequest();
//    GPSCoord gpsc = RevelesIO::instance()->GetLastGPSCoord();
    MapNode *mn = RevelesMap::instance()->GetNodeFromCoord(currentLocation);

    if(mn != NULL)
    {
        if(currentNode->mapX = mn->mapX)
        {
            if(currentNode->mapY == mn->mapY)
            {
                return true;
            }

            return false;
        }

        return false;
    }

    return false;
}

void NavigationAssisiant::DeadReckon()
{
    // Need to find the relation between travel speed,
    // and degrees of lat/long changed.
    // This will also need to be adjusted with vectoring as well.
}
