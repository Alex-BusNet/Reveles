/*
 * This file was generated by qdbusxml2cpp version 0.8
 * Command line was: qdbusxml2cpp -c RevelesDBusCoreInterface -p reveles_dbus_core_interface.h:reveles_dbus_core_interface.cpp -i Common/datatypes.h revelesdbus.xml
 *
 * qdbusxml2cpp is Copyright (C) 2017 The Qt Company Ltd.
 *
 * This is an auto-generated file.
 * Do not edit! All changes made to it will be lost.
 */

#ifndef REVELES_DBUS_CORE_INTERFACE_H
#define REVELES_DBUS_CORE_INTERFACE_H

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
class RevelesDBusCoreInterface: public QDBusAbstractInterface
{
    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "com.reveles.RevelesCoreInterface"; }

public:
    RevelesDBusCoreInterface(const QString &service, const QString &path, const QDBusConnection &connection, QObject *parent = nullptr);

    ~RevelesDBusCoreInterface();

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
};

namespace com {
  namespace reveles {
    typedef ::RevelesDBusCoreInterface RevelesCoreInterface;
  }
}
#endif