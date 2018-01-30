#include "revelesio.h"
#include <iostream>
#include <random>
#include <math.h>

Q_GLOBAL_STATIC(RevelesIO, rio)

RevelesIO *RevelesIO::instance()
{
    return rio;
}

void RevelesIO::initIO()
{
    wiringPiSetup();
    pinMode(SEL_A, OUTPUT);
    pinMode(SEL_B, OUTPUT);
    pinMode(ECHO, INPUT);
    pinMode(SIG_1, INPUT);
    pinMode(SIG_2, INPUT);
    pinMode(TRIG, OUTPUT);
    /// TODO: Add pins for ToF

    wiringPiI2CSetup(ARDUINO);

    agm = new LSM9DS1();
    agm->setup();

    XGAvailable = agm->AccelGyroFound();
    MagAvailable = agm->MagFound();

    isrWait = false;
}

RevelesIO::RevelesIO()
{

}

void RevelesIO::SendMotorUpdate(float usDist, float tofDist)
{
    /// TODO: Figure out I2C Comm format
    wiringPiI2CWrite(ARDUINO, usDist);
}

GPSCoord RevelesIO::ReadGPS()
{    
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

    if(((micros() - trigStart) > TIMEOUT)) { emit echoReady(-1, ""); std::cout << "timeout 1" << std::endl; return -1;}

    ping = micros();

    while(digitalRead(ECHO) == HIGH && ((micros() - trigStart) < TIMEOUT)) {;}

    if(((micros() - trigStart) > TIMEOUT)) { emit echoReady(-1, ""); std::cout << "timeout 2" << std::endl; return -1; }

    pong = micros();

    dist = (float)(pong - ping) * 0.017150;
    dist /= 2.5;

    if( dist < 12 )
        emit echoReady(dist, "in");
    else
    {
        dist /= 12;
        emit echoReady(dist, "ft");
    }

    return dist;
}

void RevelesIO::TriggerTimeOfFlight()
{
    // TODO: setup trigger conditions.
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
    if(!XGAvailable) { return AccelDirection {0.0f, 0.0f, 0.0f}; }

    return agm->ReadAccel();
}

GyroDirection RevelesIO::ReadGyroscope()
{
    if(!XGAvailable) { return GyroDirection {0.0f, 0.0f, 0.0f}; }

    return agm->ReadGyro();
}
