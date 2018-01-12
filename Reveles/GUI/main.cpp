#include "revelesgui.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RevelesGui w;
    w.show();

    return a.exec();
}
