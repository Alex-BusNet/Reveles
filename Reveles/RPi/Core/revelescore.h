#ifndef REVELESCORE_H
#define REVELESCORE_H

#include <QObject>
#include <QTimer>
#include "reveles_dbus_adaptor.h"
#include "../Common/datatypes.h"
#include "IO/revelesio.h"
#include "Core/analyticalengine.h"
#include "Core/navigationassisiant.h"
#include "Core/objectdetector.h"
#include "revelesmap.h"

class AnalyticalEngine;

class RevelesCore : public QObject
{
    Q_OBJECT
public:
    RevelesCore(RevelesDBusAdaptor *dbusAdaptor = 0);
    ~RevelesCore();

private:
    QTimer *coreTimer;
    int updateInterval;
    bool commsGood;
    GPSCoord dest, loc;
    RevelesDBusAdaptor *rdba;

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
    void close();

signals:
    void usTriggered();
    void currentLocation(GPSCoord gpsc);
    void commResponse(bool good);

};

#endif // REVELESCORE_H
