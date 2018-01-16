#include "revelescore.h"
#include <iostream>

using namespace  std;

RevelesCore::RevelesCore(RevelesDBusAdaptor *dbusAdaptor) :
    rdba(dbusAdaptor)
{
    // Inbound comms (GUI -> CORE)
    connect(rdba, SIGNAL(commQuery()), this, SLOT(commCheck()));
    connect(rdba, SIGNAL(requestCurrentLocation()), this, SLOT(updateMapData()));
    connect(rdba, SIGNAL(setMapUpdateInterval(int)), this, SLOT(setMapUpdateInterval(int)));
    connect(rdba, SIGNAL(requestMapUpdate()), this, SLOT(updateMapData()));

    // Outbound comms (CORE -> GUI)
    connect(this, SIGNAL(commResponse(bool)), rdba, SIGNAL(commResponse(bool)));
    connect(this, SIGNAL(currentLocation(GPSCoord)), rdba, SIGNAL(locationUpdate(GPSCoord)));

    // Additional comms (CORE -> CORE)
    connect(this, SIGNAL(currentLocation(GPSCoord)), RevelesAnalyticalEngine::instance(), SLOT(updateLocation(GPSCoord)));

    // Variable Init
    RevelesAnalyticalEngine::instance()->Init();
    RevelesIO::instance()->initIO();
    commsGood = false;
    updateInterval = 1000;

    /// TODO: Change this to ping GPS module for current location
    dest = GPSCoord{0, 0};
    loc = dest;

    coreTimer = new QTimer();
    coreTimer->setInterval(updateInterval);
    connect(coreTimer, SIGNAL(timeout()), this, SLOT(updateMapData()));
    coreTimer->start();

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
    RevelesAnalyticalEngine::instance()->Start(dest);
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

void RevelesCore::readSensor()
{
    RevelesIO::instance()->triggerUltrasonic(0b001);
    emit usTriggered();
}

void RevelesCore::updateMapData()
{
    loc = RevelesIO::instance()->ReadGPS();
    emit currentLocation(loc);
}

