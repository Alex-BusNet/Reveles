#ifndef REVELESCORE_H
#define REVELESCORE_H

#include "rpi.h"
#include <QTimer>
#include <QtGui/QKeyEvent>
#include <iostream>
#include <iomanip>

#include "revelesmap.h"
#include "reveles_dbus_adaptor.h"
#include "reveles_dbus_interface.h"

#include "Core/analyticalengine.h"
#include "Core/navigationassisiant.h"
#include "Core/objectdetector.h"
#include "IO/revelesio.h"

class AnalyticalEngine;

class RevelesCore : public QObject
{
    Q_OBJECT
public:
    RevelesCore(RevelesDBusAdaptor *dbusAdaptor = 0, com::reveles::RevelesCoreInterface *iface = 0);
    ~RevelesCore();

    RevelesDBusAdaptor *rdba;
    com::reveles::RevelesCoreInterface *rci;

private:
    QTimer *coreTimer;
    int updateInterval;
    bool commsGood, active;
    GPSCoord dest, loc;

public slots:
    void commCheck();
    void setDestination(GPSCoord gpsc);
    void setMapUpdateInterval(int milliseconds);
    void getCurrentLocation();
    void ForceEndNav();

private:
    void updateOrientation();
    void readAGM();

private slots:
    void readSensor();
    void updateMapData();
    void coreLoop();
    void closeCore();

signals:
    void usTriggered();
    void currentLocation(GPSCoord gpsc);
    void commResponse(bool good);
    void sendAGStatus(bool good);
    void sendMagStatus(bool good);
    void sendAccelReading(AccelDirection ad);
    void sendMagReading(MagDirection md);
    void sendGyroReading(GyroDirection gd);
};

class RevelesEventFilter : public QObject
{
    Q_OBJECT
public:
    RevelesEventFilter(RevelesCore *parent);

    bool eventFilter(QObject *watched, QEvent *event);
private:
    RevelesCore *rc;
};

#endif // REVELESCORE_H
