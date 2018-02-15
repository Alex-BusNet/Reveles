#include "revelesio.h"
#include <iostream>
#include <random>
#include <math.h>
#include "Common/logger.h"
#include "Common/messages.h"
#include <cstdlib>

Q_GLOBAL_STATIC(RevelesIO, rio)
#define B2STR( x ) (x ? "True" : "False")

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

    // Need to play with this a bit. The ISR is triggering way too much.
//    wiringPiISR(RECIEVE_READY, INT_EDGE_RISING, GPS_Response_Ready);

    arduinoFound = false;
    fdArduino = wiringPiI2CSetup(ARDUINO);

    wiringPiI2CWrite(fdArduino, COM_CHECK);
    uint8_t res = wiringPiI2CRead(fdArduino);

    if((res & COM_CHECK) == COM_CHECK)
        arduinoFound = true;
    else
        arduinoFound = false;

    Logger::writeLine(instance(), QString("Arduino found at 0x%1: %2").arg(ARDUINO, 2, 16, QChar('0')).arg(B2STR(arduinoFound)));

    lastKnownCoord = GPSCoord{0, 0};
    agm = new LSM9DS1();
    agm->setup();

    XGAvailable = agm->AccelGyroFound();
    MagAvailable = agm->MagFound();

    isrWait = false;
    dist = 0;
    inch = 156;
    motorDir = M_FWD;
	servoDir = RET_NEUTRAL;

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

    // Total time to transmit: 425ms
    wiringPiI2CWrite(fdArduino, START);
    delay(85);
    wiringPiI2CWrite(fdArduino, CMD_M);
    delay(85);
    wiringPiI2CWrite(fdArduino, inch);
    delay(85);
    wiringPiI2CWrite(fdArduino, motorDir);
    delay(85);
    wiringPiI2CWrite(fdArduino, tofDist);
    delay(85);
    wiringPiI2CWrite(fdArduino, END);

    Logger::writeLine(instance(), QString("START:         0x%1").arg(START, 2, 16, QChar('0')));
    Logger::writeLine(instance(), QString("Motor Command: 0x%1").arg(CMD_M, 2, 16, QChar('0')));
    Logger::writeLine(instance(), QString("US Dist:       %1 in").arg(inch, 4, 10, QChar('0')));
    Logger::writeLine(instance(), QString("Motor Dir:     0x%1").arg(motorDir, 2, 16, QChar('0')));
    Logger::writeLine(instance(), QString("ToF Dist:      %1 in").arg(tofDist, 4, 10, QChar('0')));
    Logger::writeLine(instance(), QString("END:           0x%1").arg(END, 2, 16, QChar('0')));
}

void RevelesIO::SetMotorDirection(uint8_t dir)
{
    motorDir = dir;
    SendMotorUpdate();
}

void RevelesIO::SendGPSRequest()
{
    wiringPiI2CWrite(fdArduino, START);
    delay(85);
    wiringPiI2CWrite(fdArduino, CMD_G);
    delay(85);
    wiringPiI2CWrite(fdArduino, END);

    Logger::writeLine(instance(), Reveles::I2C_GPS_SEND.arg(START, 2, 16, QChar('0')));
    Logger::writeLine(instance(), Reveles::I2C_GPS_SEND.arg(CMD_G, 2, 16, QChar('0')));
    Logger::writeLine(instance(), Reveles::I2C_GPS_SEND.arg(END, 2, 16, QChar('0')));
}

void RevelesIO::SendServoUpdate()
{
    int angle = 90;
    // Total time to transmit: 340ms
	wiringPiI2CWrite(fdArduino, START);
	delay(85);
	wiringPiI2CWrite(fdArduino, CMD_S);
	delay(85);
	wiringPiI2CWrite(fdArduino, servoDir);
	delay(85);
	
	if(servoDir == TURN_LEFT)
        angle = 180;
	else if(servoDir == TURN_RIGHT)
        angle = 0;
	else
        angle = 90;

    wiringPiI2CWrite(fdArduino, angle);
	delay(85);
	wiringPiI2CWrite(fdArduino, END);

    Logger::writeLine(instance(), Reveles::I2C_SERVO_SEND.arg(START, 2, 16, QChar('0')));
    Logger::writeLine(instance(), Reveles::I2C_SERVO_SEND.arg(CMD_S, 2, 16, QChar('0')));
    Logger::writeLine(instance(), Reveles::I2C_SERVO_SEND.arg(servoDir, 2, 16, QChar('0')));
    Logger::writeLine(instance(), Reveles::I2C_SERVO_ANGLE.arg(angle, 3, 10, QChar('0')));
    Logger::writeLine(instance(), Reveles::I2C_SERVO_SEND.arg(END, 2, 16, QChar('0')));
}

void RevelesIO::SetServoDirection(uint8_t dir)
{
	servoDir = dir;
	SendServoUpdate();
}

GPSCoord RevelesIO::GetLastGPSCoord()
{
    return lastKnownCoord;
}

void RevelesIO::ReadGPS()
{
    GPSCoord gpsc{0,0};
    // Format: <START> -> <LATITUDE> -> <latitude value> -> <LONGITUDE> -> <longitude value> -> <END>

    Logger::writeLine(instance(), "Reading Arduino...");
    int response = wiringPiI2CRead(fdArduino);

    if(response == START)
    {
        bool readLat = false, readLong = false;
        while(response != END)
        {
            response = wiringPiI2CRead(fdArduino);
            Logger::writeLine(instance(), QString("GPS Response: %1").arg(response));

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

    Logger::writeLine(instance(), QString("GPS Response: %1, %2").arg(gpsc.latitude).arg(gpsc.longitude));
    lastKnownCoord = gpsc;
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

void GPS_Response_Ready()
{
    RevelesIO::instance()->ReadGPS();
}
