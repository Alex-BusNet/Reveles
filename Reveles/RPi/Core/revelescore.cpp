#include "revelescore.h"
#include <QCoreApplication>

//#define USE_OBJ_DETECT
//#define OBJ_DETECT_DEBUG

#define FA_SW_CORNER GPSCoord{41.631906, -85.006082}
#define FA_SE_CORNER GPSCoord{41.631910, -85.005670}
#define FA_NE_CORNER GPSCoord{41.632562, -85.005680}
#define FA_NW_CORNER GPSCoord{41.632559, -85.005982}
#define FA_MAP_NW GPSCoord{41.632559, -85.006123}
#define FA_MAP_SW GPSCoord{41.631906, -85.006123}
#define FA_MAP_SE GPSCoord{41.631906, -85.005665}

#define B2STR( x ) (x ? "True" : "False")

#if defined __linux__ && defined USE_OBJ_DETECT
#include <X11/Xlib.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#endif

using namespace std;

QFile Logger::logFile("temp");
LoggerFlags Logger::flags = NO_LOG_FLAGS;

RevelesCore::RevelesCore(RevelesDBusAdaptor *dbusAdaptor, com::reveles::RevelesCoreInterface *iface) :
    rdba(dbusAdaptor), rci(iface)
{
    this->setObjectName("RevelesCore");

    Logger::InitLog();
    Logger::SetLogFlags(SHOW_POINT | SHOW_POS_NEG | BOOL_AS_ALPHA | FIXED_LENGTH);
    Logger::SetPrecision(3);

    cout << Reveles::REVELES_CORE_INFO.arg(Reveles::REVELES_VERSION).toStdString() << endl;

    this->installEventFilter(new RevelesEventFilter(this));

    // Inbound comms (GUI -> CORE)
    connect(rci, &RevelesDBusInterface::commQuery, this, &RevelesCore::commCheck);
    connect(rci, &RevelesDBusInterface::requestCurrentLocation, this, &RevelesCore::updateMapData);
    connect(rci, &RevelesDBusInterface::requestMapUpdate, this, &RevelesCore::updateMapData);
    connect(rci, &RevelesDBusInterface::setMapUpdateInterval, this, &RevelesCore::setMapUpdateInterval);
    connect(rci, &RevelesDBusInterface::setDestination, this, &RevelesCore::setDestination);
    connect(rci, &RevelesDBusInterface::aboutToQuit, AnalyticalEngine::instance(), &AnalyticalEngine::aboutToQuit);
#ifdef USE_OBJ_DETECT
    connect(rci, SIGNAL(aboutToQuit()), ObjectDetector::instance(), SLOT(aboutToQuit()));
#endif
    connect(rci, &RevelesDBusInterface::aboutToQuit, this, &RevelesCore::closeCore);

    // Loop the aboutToQuit() signal from the /GUI object to the aboutToQuit() signal from the /Core object
    connect(rci, &RevelesDBusInterface::aboutToQuit, rdba, &RevelesDBusAdaptor::aboutToQuit);

    // Outbound comms (CORE -> GUI)
    connect(this, &RevelesCore::commResponse, rdba, &RevelesDBusAdaptor::commResponse);
    connect(this, &RevelesCore::currentLocation, rdba, &RevelesDBusAdaptor::locationUpdate);
    connect(this, &RevelesCore::sendAGStatus, rdba, &RevelesDBusAdaptor::setAGStatus);
    connect(this, &RevelesCore::sendMagStatus, rdba, &RevelesDBusAdaptor::setMagStatus);
    connect(Logger::instance(), &Logger::newMessage, rdba, &RevelesDBusAdaptor::sendLogMessage);
    connect(NavigationAssisiant::instance(), &NavigationAssisiant::PathReady, rdba, &RevelesDBusAdaptor::sendPathInfo);

    // Additional comms (CORE -> CORE)
    connect(this, &RevelesCore::currentLocation, NavigationAssisiant::instance(), &NavigationAssisiant::updateLocation);

#ifdef USE_OBJ_DETECT
    XInitThreads();
    ObjectDetector::instance()->Init();
    RevelesCamera::instance()->Init();
#endif

    // Variable Init
    AnalyticalEngine::instance()->Init();
    NavigationAssisiant::instance()->Init();
    RevelesIO::instance()->initIO();
    RevelesMap::instance()->Init();

    Logger::writeLine(this, Reveles::XG_FOUND.arg(B2STR(RevelesIO::instance()->hasXG())));
    Logger::writeLine(this, Reveles::MAG_FOUND.arg(B2STR(RevelesIO::instance()->hasMag())));
    emit sendAGStatus(RevelesIO::instance()->hasXG());
    emit sendMagStatus(RevelesIO::instance()->hasMag());

    commsGood = false;
    updateInterval = 1000;

    /// TODO: Change this to ping GPS module for current location
    dest = GPSCoord{0, 0};
    loc = FA_SE_CORNER;

#ifdef USE_OBJ_DETECT
#ifdef OBJ_DETECT_DEBUG
    namedWindow("Detector Output", WINDOW_AUTOSIZE);
#endif
    AnalyticalEngine::instance()->Start();
    ObjectDetector::instance()->Run();
#endif

    // Test the GetDistance() function
    NavigationAssisiant::instance()->updateLocation(RevelesMap::instance()->GetOffset());
    Logger::writeLine(this, QString("Testing GetDistance()..."));
    Logger::writeLine(this,
                      QString("SE corner to NE corner: %1 ft")
                      .arg(NavigationAssisiant::instance()->GetDistance(FA_SE_CORNER, FA_NE_CORNER)));

    Logger::writeLine(this, QString("Map dimensions: %1 x %2 tiles.")
                      .arg(NavigationAssisiant::instance()->GetDistance(FA_MAP_NW, FA_MAP_SW) / 2)
                      .arg(NavigationAssisiant::instance()->GetDistance(FA_MAP_SW, FA_MAP_SE) / 2));
    Logger::writeLine(this, QString("Latitude delta per tile: %1")
                      .arg((FA_MAP_NW.latitude - FA_MAP_SW.latitude) / 63.0, 11, 'F', 10, QChar('0')));
    Logger::writeLine(this, QString("Longitude delta per tile: %1")
                      .arg((FA_MAP_SE.longitude - FA_MAP_SW.longitude) / 120.0, 11, 'F', 10, QChar('0')));

    Logger::writeLine(this, QString("Testing GPS..." ));
    RevelesIO::instance()->SendGPSRequest();
    delay(1000);
    GPSCoord g = RevelesIO::instance()->GetLastGPSCoord();
    Logger::writeLine(this, QString("Response: %1, %2").arg(g.latitude).arg(g.longitude));

    coreTimer = new QTimer();
    coreTimer->setInterval(updateInterval);
    connect(coreTimer, SIGNAL(timeout()), this, SLOT(coreLoop()));
    coreTimer->start();

    active = true;
    Logger::writeLine(this, Reveles::CORE_INIT_COMPLETE);
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
    Logger::writeLine(this, Reveles::CORE_COMM_RECIEVED);
    commsGood = true;
    emit commResponse(commsGood);
}

void RevelesCore::setDestination(GPSCoord gpsc)
{
    dest = gpsc;
    NavigationAssisiant::instance()->Start(dest);
}

void RevelesCore::setMapUpdateInterval(int milliseconds)
{
    updateInterval = milliseconds;
    coreTimer->stop();
    coreTimer->setInterval(updateInterval);
    coreTimer->start();

    Logger::writeLine(this, Reveles::MAP_INTERVAL_CHANGED);
}

void RevelesCore::getCurrentLocation()
{
    // location is temporary
    emit currentLocation(FA_NW_CORNER);
}

void RevelesCore::updateOrientation()
{

}

void RevelesCore::readAGM()
{
    MagDirection md = RevelesIO::instance()->ReadMagnetometer();
//    Logger::writeLine(this, Reveles::MAG_DATA.arg(md.x, 5, 10).arg(md.y, 5, 10).arg(md.z, 5, 10));

    AccelDirection ad = RevelesIO::instance()->ReadAccelerometer();
//    Logger::writeLine(this, Reveles::ACCEL_DATA.arg(ad.x, 5, 10).arg(ad.y, 5, 10).arg(ad.z, 5, 10));

    GyroDirection gd = RevelesIO::instance()->ReadGyroscope();
//    Logger::writeLine(this, Reveles::GYRO_DATA.arg(gd.x, 5, 10).arg(gd.y, 5, 10).arg(gd.z, 5, 10));
}

void RevelesCore::readSensor()
{
    float us = RevelesIO::instance()->triggerUltrasonic(0b001);
    RevelesIO::instance()->SendMotorUpdate();
//    emit usTriggered();
}

void RevelesCore::updateMapData()
{
    RevelesIO::instance()->SendGPSRequest();
    loc = RevelesIO::instance()->GetLastGPSCoord();
    emit currentLocation(loc);
}

void RevelesCore::coreLoop()
{
    static int directionCount = 0;

//    readSensor();
//    NavigationAssisiant::instance()->Orient();
//    updateMapData();

    //=========================
    // I2C motor drive testing
    if(directionCount == 1)
        RevelesIO::instance()->SetMotorDirection(M_FWD);
    else if(directionCount == 10)
        RevelesIO::instance()->SetMotorDirection(M_STOP);
    else if (directionCount == 12)
        RevelesIO::instance()->SetMotorDirection(M_REV);
    else if(directionCount == 22)
    {
        RevelesIO::instance()->SetMotorDirection(M_STOP);
        directionCount = 0;
    }

    directionCount++;
    //=========================

//    readAGM();
}

void RevelesCore::closeCore()
{
    if(coreTimer->isActive())
    {
        coreTimer->stop();
    }

    Logger::writeLine(this, QString("Closing..."));
    Logger::ClearLogFlags();
    Logger::CloseLog();

    rdba->aboutToQuit();

    active =  false;

    if(this->parent() != NULL)
        ((QCoreApplication*)this->parent())->exit(0);
}


bool RevelesEventFilter::eventFilter(QObject *watched, QEvent *event)
{
    Logger::write(this, QString("Event Fired."));
    if(event->type() == QEvent::KeyPress)
    {
        QKeyEvent *key = static_cast<QKeyEvent*>(event);

        if(key->key() == Qt::Key_Escape)
            rc->rdba->aboutToQuit();
        else
            return QObject::eventFilter(watched, event);

        return true;
    }
    else
        return QObject::eventFilter(watched, event);

    return false;
}

RevelesEventFilter::RevelesEventFilter(RevelesCore *parent)
    : rc(parent)
{

}
