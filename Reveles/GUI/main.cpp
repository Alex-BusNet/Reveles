#include <QApplication>
#include <QDBusConnection>
#include "revelesgui.h"
#include "reveles_dbus_interface.h"

using namespace com::reveles;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RevelesCoreInterface *iface = new RevelesCoreInterface("com.reveles.core", "/Core",
                                             QDBusConnection::sessionBus());
//    RevelesGui w(iface);
//    w.show();

    RevelesGui::instance()->setDBusInterface(iface);
    RevelesGui::instance()->show();

    return a.exec();
}
