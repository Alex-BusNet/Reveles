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
#include "tof.h"
#include <mutex>
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
#define SDA 8
#define SCL 9

//-----------------------
//  I2C Device Addresses
#define ARDUINO             0x04
#define NUCLEO_FRONT        0x0A
#define NUCLEO_REAR         0x0B

// The TOF addresses may need to be bit shifted;
// if we use them from here at all.

// NOTE: Left and Right are what the
//       ToF boards treats as Left and Right.
//       NOT what left and right are relative to
//       Reveles.

/*=================================
 * Time Of Flight sensor locations
 *          0-----1-----2
 *          |   Front   |
 *          |           |
 *          |           |
 *          7           3
 *          |           |
 *          |           |
 *          |           |
 *          6-----5-----4
 *=================================
*/
#define TOF_DEFAULT_ADDR    0x29
#define TOF_I2C_ADDR_REG    0x008A

#define TOF_F_CENTER        0x36
#define TOF_F_LEFT          0x30
#define TOF_F_RIGHT         0x31
#define TOF_R_CENTER        0x37
#define TOF_R_RIGHT         0x33
#define TOF_R_LEFT          0x34
#define TOF_RIGHT           0x32
#define TOF_LEFT            0x35

// GPIO expanders connect to the Seven-Seg displays
// on the ToF breakout board. Each controls 2 of the
// 4 displays.
// NOTE: We aren't using these, I just put them here for reference. - Alex
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

    float triggerUltrasonic(uint8_t sel);
    float ReadTimeOfFlight(int sensorNum);
    bool readPIR(bool rear);

    MagDirection ReadMagnetometer();
    AccelDirection ReadAccelerometer();
    GyroDirection ReadGyroscope();

    bool hasXG();
    bool hasMag();

private:
    bool arduinoFound;
    bool nucleoFound[2] = {false, false};
    bool XGAvailable, MagAvailable;

    int fdNucleo[2];  // Array of file descriptors for Nucleo-F401RE (2)
    int fdArduino;    // File descriptor for Arduino
    int fdToF[8];     // Array of file desriptors for Time of Flight sensors. may not be needed.
    // Array of new addresses for Time of Flight sensors.
    uint8_t ToFNewAddr[8] = {0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37};

    int inch, angle;
    float dist;
    float tofDist[8];
    int8_t motorDir;
    int8_t servoDir;
    long durat;

    std::mutex tofMutex;

    QQueue<RIOData> ioRequestQueue;
    bool stopParser, stopToF;

    QFuture<void> parser, tofReader;

    GPSCoord lastKnownCoord;
    LSM9DS1 *agm;

    // Functions
    void ParseQueue();
    void SendToFRequest(int sensorNum);
    void InitToFSensors();

signals:
    void echoReady(float dist, QString unit);
    void motorDirectionUpdate(uint8_t dir);
    void arduinoStat(bool stat);
    void nucleoStat(bool stat, int idx);
    void pirStat(bool stat, bool front);

    void usReady(int idx, double value);
    void tofReady(int idx, double value);

    void servoStatus(bool front, uint8_t dir);
};

#endif // REVELESIO_H
