#include <QCoreApplication>
#include <QDBusConnection>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QDBusConnection::sessionBus().registerService("com.reveles.core");
    QDBusConnection::sessionBus().registerObject("/Core", a);

    return a.exec();
}
