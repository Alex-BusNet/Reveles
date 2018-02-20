#include <QCoreApplication>
#include <QDBusConnection>
#include <QDebug>
#include "reveles_dbus_adaptor.h"
#include "reveles_dbus_interface.h"
#include "Core/revelescore.h"
#include "Common/messages.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qRegisterMetaType<GPSCoord>("GPSCoord");
    qDBusRegisterMetaType<GPSCoord>();
    qRegisterMetaType<uint8_t>("uint8_t");

    RevelesDBusAdaptor *rdba = new RevelesDBusAdaptor(&a);
    com::reveles::RevelesCoreInterface *iface = new com::reveles::RevelesCoreInterface("com.reveles.gui", "/GUI",
                                             QDBusConnection::sessionBus());

    if(!QDBusConnection::sessionBus().registerService("com.reveles.core"))
    {
        qDebug() << Reveles::CORE_DBUS_SERVICE_REGISTER_FAIL << QDBusConnection::sessionBus().lastError();
        return -1;
    }

    if(!QDBusConnection::sessionBus().registerObject("/Core", &a))
    {
        qDebug() << Reveles::DBUS_OBJECT_REGISTER_FAIL.arg("/Core") << QDBusConnection::sessionBus().lastError();
        return -1;
    }

    RevelesCore *rc = new RevelesCore(rdba, iface);
    rc->setParent(&a);
    return a.exec();
}
