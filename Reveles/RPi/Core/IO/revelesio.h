#ifndef REVELESIO_H
#define REVELESIO_H

#include <QObject>
#include <stdint.h>
#include "RPi/Common/datatypes.h"
#include "Libraries/wiringPi/wiringPi.h"
#include "Libraries/wiringPi/wiringPiI2C.h"

//----------------------
//  Sensor GPIO pins
#define SEL_A   17
#define SEL_B   27
#define SEL_C   22
#define  ECHO   10
#define   SIG    9
#define  TRIG    4
//-----------------------

//-----------------------
//  I2C Device Addresses
#define  XG_ADDR  0x6B
#define MAG_ADDR  0x1E
#define  ARDUINO  0x04
//-----------------------

class RevelesIO : public QObject
{
    Q_OBJECT
public:
    RevelesIO();

    static RevelesIO *instance();
    void initIO();
    void SendMotorUpdate(int motorSpeedFactor);
    GPSCoord ReadGPS();
    int readSensor(uint8_t sel, SensorType type);
    void triggerUltrasonic();
};

#endif // REVELESIO_H
