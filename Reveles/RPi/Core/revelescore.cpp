#include "revelescore.h"

RevelesCore::RevelesCore()
{
    RevelesIO::instance()->initIO();

//    coreTimer = new QTimer();
//    coreTimer->setInterval(1000);
//    connect(coreTimer, SIGNAL(timeout()), this, SLOT(readSensor()));
//    coreTimer->start();
}

void RevelesCore::readSensor()
{
    RevelesIO::instance()->triggerUltrasonic(0b001);
    emit usTriggered();
}

