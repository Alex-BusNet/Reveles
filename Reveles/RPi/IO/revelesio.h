#ifndef REVELESIO_H
#define REVELESIO_H

/*
 * The purpose of RevelesIO is to act as
 * an interface between the Core and all
 * devices plugged into the IO.
 */

#include <QObject>
#include <QQueue>
#include <stdint.h>
#include <chrono>

#include "Common/datatypes.h"
#include "lsm9ds1.h"
#include <QFuture>

#include <wiringPi.h>
#include <wiringPiI2C.h>

//---------------------------
//     Sensor GPIO pins
//(use wiringPi's numbering)
#define      SEL_A  0 // GPIO-17
#define      SEL_B  2 // GPIO-27
#define       ECHO  3 // GPIO-22
#define        SIG 12 // GPIO-10
#define NUCLEO_ISR 13 // GPIO-9
#define       TRIG  7 // GPIO-4
//---------------------------

#define TIMEOUT 500000
#define RECIEVE_READY 25
#define SDA 8
#define SCL 9

//-----------------------
//  I2C Device Addresses
#define ARDUINO             0x04
#define NUCLEO_FRONT        0x2B
#define NUCLEO_REAR         0x0B

// The TOF addresses may need to be bit shifted;
// if we use them from here at all.

// NOTE: Left and Right are what the
//       ToF board treats as Left and Right.
//       NOT what left and right are relative to
//       Reveles.

#define TOF_F_CENTER        0x54
#define TOF_F_LEFT          0x56
#define TOF_F_RIGHT         0x58
#define TOF_R_CENTER        0x60
#define TOF_R_RIGHT         0x62
#define TOF_R_LEFT          0x64
#define TOF_RIGHT           0x66
#define TOF_LEFT            0x68
// GPIO expanders connect to the Seven-Seg displays
// on the ToF breakout board. Each controls 2 of the
// 4 displays.
#define GPIO_EXPANDER_U19   0x42
#define GPIO_EXPANDER_U21   0x43
//-----------------------

class LSM9DS1;

class RevelesIO : public QObject
{
    Q_OBJECT
public:
    RevelesIO();
    ~RevelesIO();
    static RevelesIO *instance();
    void initIO();

    void EnqueueRequest(RIOData riod);
    void StartNav();
    void StopNav();

    void CloseIO();

    void SendMotorUpdate();
    void SetMotorDirection(uint8_t dir);
    void SendGPSRequest();
	void SendServoUpdate();
	void SetServoDirection(uint8_t dir);

    GPSCoord GetLastGPSCoord();

    void ReadGPS();

    int triggerUltrasonic(uint8_t sel);
    int ReadTimeOfFlight(int sensorNum);
    bool readPIR(bool rear);

    MagDirection ReadMagnetometer();
    AccelDirection ReadAccelerometer();
    GyroDirection ReadGyroscope();

    bool hasXG();
    bool hasMag();

private:
    bool isrWait, arduinoFound;
    bool XGAvailable, MagAvailable;

    int fdNucleo[2];  // Array of file descriptors for Nucleo-F401RE (2)
    int fdArduino;    // File descriptor for Arduino
    int fdToF[8];     // Array of file desriptors for Time of Flight sensors.

    int dist, inch, angle, tofDist[8];
    int8_t motorDir;
    int8_t servoDir;
    long durat;

    QQueue<RIOData> ioRequestQueue;
    bool stopParser, stopToF;

    QFuture<void> parser, tofReader;

    GPSCoord lastKnownCoord;
    LSM9DS1 *agm;
//    std::chrono::steady_clock::time_point begin, end;

    // Functions
    void ParseQueue();
    void SendToFRequest(int sensorNum);

signals:
    void echoReady(float dist, QString unit);
    void motorDirectionUpdate(uint8_t dir);
    void arduinoStat(bool stat);
    void nucleoStat(bool stat, int idx);
    void pirStat(bool stat);
};

#endif // REVELESIO_H
