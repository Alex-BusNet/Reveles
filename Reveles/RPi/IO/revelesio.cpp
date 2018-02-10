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
//=========================================================================
//                 I2C Command Structure (RPi -> Arduino):
// <START> -> <M | G> -> <US reading> -> <F | S | R> -> <ToF reading> -> <END>
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
//      (Motor command only)
//==========================================================================

//---------------------------------------------------------------------------
    //Debugging section:

    QByteArray cmd;
    cmd.append("M");
    cmd.append(":");
    cmd.append(QString::number(inch));
    cmd.append(":");
    cmd.append(QString(motorDir));
    cmd.append(":");
    cmd.append(QString::number(tofDist));
    cmd.append(":");

    Logger::writeLine(instance(), Reveles::I2C_MOTOR.arg(QString(cmd)));
//---------------------------------------------------------------------------

    int cmdOut = START;
    Logger::writeLine(instance(), QString("START:         0x%1").arg(cmdOut, 8, 16, QChar('0')));
    wiringPiI2CWrite(fdArduino, cmdOut);

    cmdOut = CMD_M;
    Logger::writeLine(instance(), QString("Motor Command: 0x%1").arg(cmdOut, 8, 16, QChar('0')));
    wiringPiI2CWrite(fdArduino, cmdOut);

    cmdOut = 0x0;

    QString iParse = QString::number(inch);
    for(int i = 0; i < iParse.length(); i++)
    {
        cmdOut <<= 8;
        cmdOut |= asciiMap[iParse.at(i).toLatin1()];
    }
    wiringPiI2CWrite(fdArduino, cmdOut);
    Logger::writeLine(instance(), QString("US Dist:       0x%1").arg(cmdOut, 8, 16, QChar('0')));

    cmdOut = asciiMap[motorDir];
    Logger::writeLine(instance(), QString("Motor Dir:     0x%1").arg(cmdOut, 8, 16, QChar('0')));
    wiringPiI2CWrite(fdArduino, cmdOut);

    cmdOut = 0x0;
    iParse = QString::number(tofDist);
    for(int i = 0; i < iParse.length(); i++)
    {
        cmdOut <<= 8;
        cmdOut |= asciiMap[iParse.at(i).toLatin1()];
    }

    wiringPiI2CWrite(fdArduino, cmdOut);
    Logger::writeLine(instance(), QString("ToF Dist:      0x%1").arg(cmdOut, 8, 16, QChar('0')));

    cmdOut = END;
    wiringPiI2CWrite(fdArduino, cmdOut);

    Logger::writeLine(instance(), QString("END:           0x%1").arg(cmdOut, 8, 16, QChar('0')));
}

void RevelesIO::SetMotorDirection(char dir)
{
    motorDir = dir;
    SendMotorUpdate();
}

GPSCoord RevelesIO::ReadGPS()
{    
    int cmd = START;
    Logger::writeLine(instance(), Reveles::I2C_GPS_SEND.arg(cmd, 8, 16, QChar('0')));
    cmd = CMD_G;
    Logger::writeLine(instance(), Reveles::I2C_GPS_SEND.arg(cmd, 8, 16, QChar('0')));
    cmd = END;
    Logger::writeLine(instance(), Reveles::I2C_GPS_SEND.arg(cmd, 8, 16, QChar('0')));

    GPSCoord gpsc{0,0};

    // Wait for response.
    // Format: <START> -> <LATITUDE> -> <latitude value> -> <LONGITUDE> -> <longitude value> -> <END>
    delayMicroseconds(50);
    Logger::writeLine(instance(), "Reading Arduino...");
    int response = wiringPiI2CRead(fdArduino);

    if(response == START)
    {
        bool readLat = false, readLong = false;
        while(response != END)
        {
            response = wiringPiI2CRead(fdArduino);
            if (response == LATITUDE && !readLat)
                readLat = true;
            else if (readLat)
            {
                gpsc.latitude = response;
                readLat = false;
            }
            else if (response == LONGITUDE && !readLong)
                readLong = true;
            else if(readLong)
            {
                gpsc.longitude = response;
                readLong = false;
            }
        }
    }

    return gpsc;
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
