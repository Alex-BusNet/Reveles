#include "revelescore.h"
#include <iostream>
#include <iomanip>

//#define USE_OBJ_DETECT
#define OJB_DETECT_DEBUG

#define FA_SW_CORNER GPSCoord{41.631906, -85.006082}
#define FA_SE_CORNER GPSCoord{41.631910, -85.005670}
#define FA_NE_CORNER GPSCoord{41.632562, -85.005680}
#define FA_NW_CORNER GPSCoord{41.632559, -85.005982}

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
    connect(this, &RevelesCore::currentLocation, NavigationAssisiant::instance(), &NavigationAssisiant::updateLocation);

#ifdef USE_OBJ_DETECT
    XInitThreads();
    ObjectDetector::instance()->Init();
    RevelesCamera::instance()->Init();
#endif

    // Variable Init
    AnalyticalEngine::instance()->Init();
    RevelesIO::instance()->initIO();
    RevelesMap::instance()->Init();

    cout << boolalpha;
    cout << "[ RevelesCore ] Accelerometer/Gyroscope found: " << RevelesIO::instance()->hasXG() << endl;
    cout << "[ RevelesCore ] Magnetometer found: " << RevelesIO::instance()->hasMag() << endl;
    cout << noboolalpha;

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

    // Test the GetDistance() function
    NavigationAssisiant::instance()->updateLocation(RevelesMap::instance()->GetOffset());
    NavigationAssisiant::instance()->Start(FA_SE_CORNER);

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

void RevelesCore::readAGM()
{
    cout << setprecision(3) << showpoint << fixed << showpos;
    MagDirection md = RevelesIO::instance()->ReadMagnetometer();
    cout << "[ RevelesCore ] Mag Data:   X: " << md.x << " Y: " << md.y << " Z: " << md.z << " uT" << endl;
//    AccelDirection ad = RevelesIO::instance()->ReadAccelerometer();
//    cout << "[ RevelesCore ] Accel Data: X: " << ad.x << " Y: " << ad.y << " Z: " << ad.z << " m/s/s" << endl;
//    GyroDirection gd = RevelesIO::instance()->ReadGyroscope();
//    cout << "[ RevelesCore ] Gyro Data:  X: " << gd.x << " Y: " << gd.y << " Z: " << gd.z << " rad/s" << endl;
    cout << setprecision(6) << noshowpoint << noshowpos << endl;
    cout.unsetf(ios::fixed);
}

void RevelesCore::readSensor()
{
    float us = RevelesIO::instance()->triggerUltrasonic(0b001);
    RevelesIO::instance()->SendMotorUpdate();
//    emit usTriggered();
}

void RevelesCore::updateMapData()
{
    loc = RevelesIO::instance()->ReadGPS();
    emit currentLocation(loc);
}

void RevelesCore::coreLoop()
{
    static directionCount = 0;

//    readSensor();
    NavigationAssisiant::instance()->Orient();
    updateMapData();

    //=========================
    // I2C motor drive testing
    if(directionCount == 10)
        RevelesIO::instance()->SetMotorDirection('R');
    else if (directionCount == 20)
    {
        RevelesIO::instance()->SetMotorDirection('R');
        directionCount = 0;
    }
    else
        directionCount++;
    //=========================

    //    readAGM();
}

void RevelesCore::close()
{
    cout << "[ RevelesCore ] Closing..." << endl;
    this->close();
}

