#ifndef REVELESIO_H
#define REVELESIO_H

#include <QObject>
#include <stdint.h>
#include <chrono>
#include "Common/datatypes.h"
#include "../Libraries/wiringPi/wiringPi.h"
#include "../Libraries/wiringPi/wiringPiI2C.h"

//---------------------------
//     Sensor GPIO pins
//(use wiringPi's numbering)
#define SEL_A    0 // GPIO-17
#define SEL_B    2 // GPIO-27
#define SEL_C    3 // GPIO-22
#define  ECHO   12 // GPIO-10
#define   SIG   13 // GPIO-9
#define  TRIG    7 // GPIO-4
//---------------------------

#define TIMEOUT 500000

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
    int readSensor(SensorType type);
    void triggerUltrasonic(uint8_t sel);

    long durat;
    float dist, inch;
    bool isrWait;
    std::chrono::steady_clock::time_point begin, end;
private:

signals:
    void echoReady(float dist, QString unit);
};

#endif // REVELESIO_H
