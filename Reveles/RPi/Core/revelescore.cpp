#include "revelescore.h"
#include <iostream>

//#define USE_OBJ_DETECT

#if defined __linux__ && defined USE_OBJ_DETECT
#include <X11/Xlib.h>
#endif

using namespace  std;

RevelesCore::RevelesCore(RevelesDBusAdaptor *dbusAdaptor) :
    rdba(dbusAdaptor)
{
    // Inbound comms (GUI -> CORE)
    connect(rdba, SIGNAL(commQuery()), this, SLOT(commCheck()));
    connect(rdba, SIGNAL(requestCurrentLocation()), this, SLOT(updateMapData()));
    connect(rdba, SIGNAL(setMapUpdateInterval(int)), this, SLOT(setMapUpdateInterval(int)));
    connect(rdba, SIGNAL(requestMapUpdate()), this, SLOT(updateMapData()));
    connect(rdba, SIGNAL(aboutToQuit()), AnalyticalEngine::instance(), SLOT(aboutToQuit()));
    connect(rdba, SIGNAL(aboutToQuit()), ObjectDetector::instance(), SLOT(aboutToQuit()));
    connect(rdba, SIGNAL(aboutToQuit()), this, SLOT(close()));

    // Outbound comms (CORE -> GUI)
    connect(this, SIGNAL(commResponse(bool)), rdba, SIGNAL(commResponse(bool)));
    connect(this, SIGNAL(currentLocation(GPSCoord)), rdba, SIGNAL(locationUpdate(GPSCoord)));

    // Additional comms (CORE -> CORE)
//    connect(this, SIGNAL(currentLocation(GPSCoord)), AnalyticalEngine::instance(), SLOT(updateLocation(GPSCoord)));

#ifdef USE_OBJ_DETECT
    XInitThreads();
    ObjectDetector::instance()->Init();
#endif

    // Variable Init
    AnalyticalEngine::instance()->Init();
    RevelesIO::instance()->initIO();

    commsGood = false;
    updateInterval = 1000;

    /// TODO: Change this to ping GPS module for current location
    dest = GPSCoord{0, 0};
    loc = dest;

    coreTimer = new QTimer();
    coreTimer->setInterval(updateInterval);
    connect(coreTimer, SIGNAL(timeout()), this, SLOT(coreLoop()));
    coreTimer->start();

#ifdef USE_OBJ_DETECT
    ObjectDetector::instance()->PeopleDetect();
#endif

    cout << "RevelesCore init complete." << endl;
}

void RevelesCore::commCheck()
{
    cout << "Signal recieved from GUI" << endl;
    commsGood = true;
    emit commResponse(commsGood);
}

void RevelesCore::setDestination(GPSCoord gpsc)
{
    cout << "Setting Target Destination to " << gpsc.latitude << ", " << gpsc.longitude << endl;
    dest = gpsc;
    NavigationAssisiant::instance()->Start(dest);
}

void RevelesCore::setMapUpdateInterval(int milliseconds)
{
    cout << "Changing Map Update Interval" << endl;

    updateInterval = milliseconds;
    coreTimer->stop();
    coreTimer->setInterval(updateInterval);
    coreTimer->start();

    cout << "--Done" << endl;
}

void RevelesCore::getCurrentLocation()
{
    emit currentLocation(loc);
}

void RevelesCore::updateOrientation()
{
}

void RevelesCore::readSensor()
{
    float us = RevelesIO::instance()->triggerUltrasonic(0b001);
    RevelesIO::instance()->SendMotorUpdate(us, 0.0f);
//    emit usTriggered();
}

void RevelesCore::updateMapData()
{
    loc = RevelesIO::instance()->ReadGPS();
    emit currentLocation(loc);
}

void RevelesCore::coreLoop()
{
//    readSensor();
    NavigationAssisiant::instance()->Orient();
    updateMapData();
}

void RevelesCore::close()
{
    cout << "RevelesCore closing..." << endl;
    this->close();
}

