#ifndef REVELESCOREADAPTOR_H
#define REVELESCOREADAPTOR_H

#include <QtDBus/QDBusAbstractAdaptor>

class RevelesCoreAdaptor : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.reveles.core")

private:

public:

public slots:

signals:


};

#endif // REVELESCOREADAPTOR_H
