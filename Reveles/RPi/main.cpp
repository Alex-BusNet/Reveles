#include <QCoreApplication>
#include <QtDBus/QDBusConnection>
#include "Core/revelescore.h"
#include "reveles_dbus_adaptor.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qRegisterMetaType<GPSCoord>("GPSCoord");
    qDBusRegisterMetaType<GPSCoord>();

    RevelesDBusAdaptor *rdba = new RevelesDBusAdaptor(&a);

    QDBusConnection::sessionBus().registerService("com.reveles.core");
    QDBusConnection::sessionBus().registerObject("/Core", &a);

    RevelesCore *rc = new RevelesCore(rdba);

    return a.exec();
}
