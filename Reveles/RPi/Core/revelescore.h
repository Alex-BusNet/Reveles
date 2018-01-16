#ifndef REVELESCORE_H
#define REVELESCORE_H

#include <QObject>
#include <QTimer>
#include "reveles_dbus_adaptor.h"
#include "../Common/datatypes.h"
#include "IO/revelesio.h"
#include "RevelesAnalyticalEngine.h"

class RevelesAnalyticalEngine;

class RevelesCore : public QObject
{
    Q_OBJECT
public:
    RevelesCore(RevelesDBusAdaptor *dbusAdaptor = 0);

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

private slots:
    void readSensor();
    void updateMapData();
signals:
    void usTriggered();
    void currentLocation(GPSCoord gpsc);
    void commResponse(bool good);

};

#endif // REVELESCORE_H
