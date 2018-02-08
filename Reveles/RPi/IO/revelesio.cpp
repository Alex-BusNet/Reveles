#include "revelesio.h"
#include <iostream>
#include <random>
#include <math.h>
#include "Common/logger.h"
#include "Common/messages.h"
#include <cstdlib>

Q_GLOBAL_STATIC(RevelesIO, rio)

RevelesIO *RevelesIO::instance()
{
    return rio;
}

void RevelesIO::initIO()
{
    instance()->setObjectName("RevelesIO");

    wiringPiSetup();
    pinMode(SEL_A, OUTPUT);
    pinMode(SEL_B, OUTPUT);
    pinMode(ECHO, INPUT);
    pinMode(SIG_1, INPUT);
    pinMode(SIG_2, INPUT);
    pinMode(TRIG, OUTPUT);
    /// TODO: Add pins for ToF

    fdArduino = wiringPiI2CSetup(ARDUINO);

    agm = new LSM9DS1();
    agm->setup();

    XGAvailable = agm->AccelGyroFound();
    MagAvailable = agm->MagFound();

    isrWait = false;
    dist = 0;
    inch = 0;
    motorDir = 'F';

     // Placeholder, any value less than 48 will trigger E-Stop on the Arduino
    tofDist = 50; // inches
}

RevelesIO::RevelesIO()
{

}

void RevelesIO::SendMotorUpdate()
{
//=================================================================
//             I2C Command Structure (RPi -> Arduino):
//         (Function):(US value):(Direction):(ToF value):
// ----------------------------------------------------------------
// Possible values
//    Function:
//      (M)otor - RPi is sending drive instructions.
//      (G)PS - RPi is requesting GPS coordinates.
//    US Value: Floating Point value for PWM signal.
//      (Motor command only)
//      Value is in inches.
//      Value is not adjusted to fit between 255 and 0.
//    Direction: (Motor command only)
//      (F)orward.
//      (S)top.
//      (R)everse.
//    ToF value: Floating point value from Time of Flight sensors.
//      (Motor command only)\
//=================================================================

    QByteArray cmd;
    cmd.append("M:");
    cmd.append(QString::number(inch) + ":");
    cmd.append(QString(motorDir) + ":");
    cmd.append(QString::number(tofDist) + ":");

    Logger::writeLine(instance(), Reveles::I2C_MOTOR.arg(QString(cmd)));
    Logger::writeLine(instance(), QString("cmd as int: ") + cmd.toHex());

    /// Need to figure out how to turn the cmd string into int.
//    wiringPiI2CWrite(fdArduino, cmd.toHex());
}

void RevelesIO::SetMotorDirection(char dir)
{
    motorDir = dir;
    SendMotorUpdate();
}

GPSCoord RevelesIO::ReadGPS()
{    
    QString cmd = "G:";
    Logger::writeLine(instance(), Reveles::I2C_GPS_SEND.arg(cmd));

    // Stubbed GPS module.
    return GPSCoord{
        ((static_cast<double>(rand()) / RAND_MAX) * 180.0),
        ((static_cast<double>(rand()) / RAND_MAX) * 180.0)
    };
}

/*
 * triggerUltrasonic written based of hc-sr04.c
 * from https://github.com/dmeziere/rpi-hc-sr04/blob/master/util/hc-sr04.c
 */
float RevelesIO::triggerUltrasonic(uint8_t sel)
{
    digitalWrite(TRIG, LOW);
    delay(50);

    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    // Check this, I don't believe it
    // works the way I think it does. -Alex 1/21/18
    digitalWrite(SEL_A, 0b1 & sel);         // Get the A select bit. This should already by in index 0
    digitalWrite(SEL_B, (0b10 & sel) >> 1); // Get the B select bit and shift into index 0

    unsigned long ping, pong, trigStart;

    trigStart = micros();

    while(digitalRead(ECHO) == LOW && ((micros() - trigStart) < TIMEOUT)){;}

    if(((micros() - trigStart) > TIMEOUT)) { emit echoReady(-1, ""); return -1;}

    ping = micros();

    while(digitalRead(ECHO) == HIGH && ((micros() - trigStart) < TIMEOUT)) {;}

    if(((micros() - trigStart) > TIMEOUT)) { emit echoReady(-1, ""); return -1; }

    pong = micros();

    dist = (float)(pong - ping) * 0.017150;
    dist /= 2.5; // Convert to inches.

    // Currently used for D-Bus comms back to GUI.
    emit echoReady(dist, "in");

    return dist;
}

void RevelesIO::TriggerTimeOfFlight()
{
    // TODO: setup trigger conditions.
    tofDist = 50;
}

int RevelesIO::readPIR(bool rear)
{
    if(rear)
        return digitalRead(SIG_2);
    else
        return digitalRead(SIG_1);
}

MagDirection RevelesIO::ReadMagnetometer()
{
    if(!MagAvailable) { return MagDirection{0.0f, 0.0f, 0.0f}; }

    return agm->ReadMag();
}

AccelDirection RevelesIO::ReadAccelerometer()
{
    if(!XGAvailable) { return AccelDirection{0.0f, 0.0f, 0.0f}; }

    return agm->ReadAccel();
}

GyroDirection RevelesIO::ReadGyroscope()
{
    if(!XGAvailable) { return GyroDirection{0.0f, 0.0f, 0.0f}; }

    return agm->ReadGyro();
}

bool RevelesIO::hasXG()
{
    return this->XGAvailable;
}

bool RevelesIO::hasMag()
{
    return this->MagAvailable;
}
