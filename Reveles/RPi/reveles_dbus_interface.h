/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c RevelesDBusInterface -p reveles_dbus_interface.h:reveles_dbus_interface.cpp -i ../RPi/Common/datatypes.h revelesdbus.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef REVELES_DBUS_INTERFACE_H
#define REVELES_DBUS_INTERFACE_H

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include "Common/datatypes.h"

/*
 * Proxy class for interface com.reveles.RevelesCoreInterface
 */
class RevelesDBusInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.reveles.RevelesCoreInterface"; }

public:
    RevelesDBusInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~RevelesDBusInterface();

public Q_SLOTS: // METHODS
Q_SIGNALS: // SIGNALS
    void AutoMapUpdate(GPSCoord gpsc);
    void aboutToQuit();
    void commQuery();
    void commResponse(bool good);
    void locationUpdate(GPSCoord loc);
    void requestCurrentLocation();
    void requestMapUpdate();
    void setDestination(GPSCoord gpsc);
    void setMapUpdateInterval(int milliseconds);
    void sendLogMessage(QString msg);
};

namespace com {
  namespace reveles {
    typedef ::RevelesDBusInterface RevelesCoreInterface;
  }
}
#endif
