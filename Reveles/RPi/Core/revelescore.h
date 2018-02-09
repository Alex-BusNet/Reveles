#ifndef REVELESCORE_H
#define REVELESCORE_H

#include <QObject>
#include <QTimer>
#include <iostream>
#include <iomanip>
#include <QtGui/QKeyEvent>
#include "revelesmap.h"
#include "reveles_dbus_adaptor.h"
#include "reveles_dbus_interface.h"
#include "Common/datatypes.h"
#include "Common/logger.h"
#include "Common/messages.h"
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
