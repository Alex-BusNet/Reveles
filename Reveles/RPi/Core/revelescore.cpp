#include "revelescore.h"
#include <iostream>

//#define USE_OBJ_DETECT
#define OJB_DETECT_DEBUG

#if defined __linux__ && defined USE_OBJ_DETECT
#include <X11/Xlib.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
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
    RevelesCamera::instance()->Init();
#endif

    // Variable Init
    AnalyticalEngine::instance()->Init();
    RevelesIO::instance()->initIO();

    commsGood = false;
    updateInterval = 1000;

    /// TODO: Change this to ping GPS module for current location
    dest = GPSCoord{0, 0};
    loc = dest;

#ifdef USE_OBJ_DETECT
#ifdef OJB_DETECT_DEBUG
    namedWindow("Detector Output", WINDOW_AUTOSIZE);
#endif
    AnalyticalEngine::instance()->Start();
    ObjectDetector::instance()->Run();
#endif

    coreTimer = new QTimer();
    coreTimer->setInterval(updateInterval);
    connect(coreTimer, SIGNAL(timeout()), this, SLOT(coreLoop()));
    coreTimer->start();

    cout << "[ RevelesCore ] Init complete." << endl;
}

RevelesCore::~RevelesCore()
{
    destroyAllWindows();
    if(coreTimer != NULL && coreTimer->isActive())
    {
        coreTimer->stop();
        delete coreTimer;
    }
}

void RevelesCore::commCheck()
{
    cout << "[ RevelesCore ] Signal recieved from GUI" << endl;
    commsGood = true;
    emit commResponse(commsGood);
}

void RevelesCore::setDestination(GPSCoord gpsc)
{
    cout << "[ RevelesCore ] Setting Target Destination to " << gpsc.latitude << ", " << gpsc.longitude << endl;
    dest = gpsc;
    NavigationAssisiant::instance()->Start(dest);
}

void RevelesCore::setMapUpdateInterval(int milliseconds)
{
    updateInterval = milliseconds;
    coreTimer->stop();
    coreTimer->setInterval(updateInterval);
    coreTimer->start();

    cout << "[ RevelesCore ] Map Update Interval changed." << endl;
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
    cout << "[ RevelesCore ] Closing..." << endl;
    this->close();
}

