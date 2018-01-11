//#include "RPi/Core/mainwidget.h"
#include "GUI/RevelesGUI/revelesgui.h"
#include <QApplication>
//#include <QDBusConnection>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWidget w(NULL, argc, argv);

//    if( !QDBusConnection::sessionBus().isConnected())
//    {
//        fprintf(stderr, "Cannot connect to the D-Bus session bus.");
//        return 1;
//    }

    RevelesGui::instance()->show();

    return a.exec();
}
