//#include "RPi/Core/mainwidget.h"
#include "GUI/RevelesGUI/revelesgui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    MainWidget w(NULL, argc, argv);
    RevelesGui::instance()->show();

    return a.exec();
    return 0;
}
