#include "revelesgui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RevelesGui::instance()->show();

    return a.exec();
}
