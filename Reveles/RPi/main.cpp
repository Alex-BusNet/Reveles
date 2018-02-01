#include <QCoreApplication>
#include <QtDBus/QDBusConnection>
#include <QDebug>
#include "Core/revelescore.h"
#include "reveles_dbus_adaptor.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qRegisterMetaType<GPSCoord>("GPSCoord");
    qDBusRegisterMetaType<GPSCoord>();

    RevelesDBusAdaptor *rdba = new RevelesDBusAdaptor(&a);

    if(!QDBusConnection::sessionBus().registerService("com.reveles.core"))
    {
        qDebug() << "Failed to register service" << QDBusConnection::sessionBus().lastError();
        return -1;
    }

    if(!QDBusConnection::sessionBus().registerObject("/Core", &a))
    {
        qDebug() << "Failed to register object" << QDBusConnection::sessionBus().lastError();
        return -1;
    }

    RevelesCore *rc = new RevelesCore(rdba);

    return a.exec();
}
