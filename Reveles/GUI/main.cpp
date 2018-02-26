#include <QApplication>
#include <QDBusConnection>
#include <QDebug>
#include "reveles_dbus_adaptor.h"
#include "reveles_dbus_interface.h"
#include "revelesgui.h"

using namespace com::reveles;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<GPSCoord>("GPSCoord");
    qDBusRegisterMetaType<GPSCoord>();
    qRegisterMetaType<uint8_t>("uint8_t");
    qRegisterMetaType<MagDirection>("MagDirection");
    qRegisterMetaType<AccelDirection>("AccelDirection");
    qRegisterMetaType<GyroDirection>("GyroDirection");
    qDBusRegisterMetaType<uint8_t>();
    qDBusRegisterMetaType<MagDirection>();
    qDBusRegisterMetaType<AccelDirection>();
    qDBusRegisterMetaType<GyroDirection>();

    RevelesDBusAdaptor *rda = new RevelesDBusAdaptor(&a);
    RevelesCoreInterface *iface = new RevelesCoreInterface("com.reveles.core", "/Core",
                                             QDBusConnection::sessionBus());

    if(!QDBusConnection::sessionBus().registerService("com.reveles.gui"))
    {
        qDebug() << "[ RevelesGUI ] Failed to register service" << QDBusConnection::sessionBus().lastError();
        return -1;
    }

    if(!QDBusConnection::sessionBus().registerObject("/GUI", &a))
    {
        qDebug() << "[ RevelesGUI ] Failed to register object" << QDBusConnection::sessionBus().lastError();
        return -1;
    }

    RevelesGui::instance()->setDBusAdaptor(rda);
    RevelesGui::instance()->setDBusInterface(iface);
    RevelesGui::instance()->show();

    return a.exec();
}
