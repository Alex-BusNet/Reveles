#ifndef REVELESCORE_H
#define REVELESCORE_H

#include "../Common/datatypes.h"
#include "RPi/Core/IO/revelesio.h"
#include <QTimer>
#include <QObject>

class RevelesCore : public QObject
{
    Q_OBJECT
public:
    RevelesCore();

private:
    QTimer *coreTimer;

private slots:
    void readSensor();

signals:
    void usTriggered();

};

#endif // REVELESCORE_H
