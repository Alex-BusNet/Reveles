#include <QApplication>
#include <QDBusConnection>

#include "keyboard.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    if( !QDBusConnection::sessionBus().registerService("com.reveles.inputmethod") )
    {
        qFatal("Unable to register virtual keyboard service at DBus.");
        return 0;
    }

    Keyboard kb;

    if ( !QDBusConnection::sessionBus().registerObject("/VirtualKeyboard", &kb, QDBusConnection::ExportAllSignals | QDBusConnection::ExportAllSlots) )
    {
        qFatal("Unable to register virtual keyboard object at DBus");
    }

    return a.exec();
}
