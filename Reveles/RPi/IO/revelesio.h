#ifndef REVELESIO_H
#define REVELESIO_H

/*
 * The purpose of RevelesIO is to act as
 * an interface between the Core and all
 * devices plugged into the IO.
 */

#include <QObject>
#include <stdint.h>
#include <chrono>

#include "Common/datatypes.h"
#include "lsm9ds1.h"

#include "../Libraries/wiringPi/wiringPi.h"
#include "../Libraries/wiringPi/wiringPiI2C.h"

//---------------------------
//     Sensor GPIO pins
//(use wiringPi's numbering)
#define SEL_A    0 // GPIO-17
#define SEL_B    2 // GPIO-27
#define  ECHO    3 // GPIO-22
#define SIG_1   12 // GPIO-10
#define SIG_2   13 // GPIO-9
#define  TRIG    7 // GPIO-4
//---------------------------

#define TIMEOUT 500000

//-----------------------
//  I2C Device Addresses
//#define  XG_ADDR  0x6B <-- moved to lsm9ds1.h
//#define MAG_ADDR  0x1E <-|
#define  ARDUINO  0x04

/// TODO: Add ToF I2C addresses
//-----------------------

class RevelesIO : public QObject
{
    Q_OBJECT
public:
    RevelesIO();
    static RevelesIO *instance();
    void initIO();

    void SendMotorUpdate(float usDist, float tofDist);

    GPSCoord ReadGPS();

    float triggerUltrasonic(uint8_t sel);
    void TriggerTimeOfFlight();

    int readPIR(bool rear);

    MagDirection ReadMagnetometer();
    AccelDirection ReadAccelerometer();
    GyroDirection ReadGyroscope();

private:
    bool isrWait;
    bool XGAvailable, MagAvailable;
    float dist, inch;
    long durat;

    LSM9DS1 *agm;

    std::chrono::steady_clock::time_point begin, end;

signals:
    void echoReady(float dist, QString unit);
};

#endif // REVELESIO_H
