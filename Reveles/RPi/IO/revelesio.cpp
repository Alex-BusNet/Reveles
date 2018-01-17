#include "revelesio.h"
#include <iostream>
#include <random>

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
    pinMode(SEL_C, OUTPUT);
    pinMode(ECHO, INPUT);
    pinMode(SIG, INPUT);
    pinMode(TRIG, OUTPUT);

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

void RevelesIO::SendMotorUpdate(int motorSpeedFactor)
{
    wiringPiI2CWrite(ARDUINO, motorSpeedFactor);
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
 * This function may be removed at a later time
 * once all the sensors are sorted out. I feel
 * this may be a redundant function that is not
 * needed. -Alex 1/17/2018
 */
int RevelesIO::readSensor(SensorType type)
{
    if(type == US)
    {
        return dist;
    }
    else if (type == PIR)
    {
        return digitalRead(SIG);
    }
    else if(type == TOF)
    {
        // TODO: Determine if Time of Flight sensors will be used.
        //       if so, determine how data formatted.
        return 0;
    }
    else
    {
        return 0;
    }

}

/*
 * triggerUltrasonic written based of hc-sr04.c
 * from https://github.com/dmeziere/rpi-hc-sr04/blob/master/util/hc-sr04.c
 */
void RevelesIO::triggerUltrasonic(uint8_t sel)
{
    digitalWrite(TRIG, LOW);
    delay(50);

    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    digitalWrite(SEL_A, 0b001 & sel);
    digitalWrite(SEL_B, 0b010 & sel);
    digitalWrite(SEL_C, 0b100 & sel);

    unsigned long ping, pong, trigStart;

    trigStart = micros();

    while(digitalRead(ECHO) == LOW && ((micros() - trigStart) < TIMEOUT)){;}

    if(((micros() - trigStart) > TIMEOUT)) { emit echoReady(-1, ""); std::cout << "timeout 1" << std::endl; return; }

    ping = micros();

    while(digitalRead(ECHO) == HIGH && ((micros() - trigStart) < TIMEOUT)) {;}

    if(((micros() - trigStart) > TIMEOUT)) { emit echoReady(-1, ""); std::cout << "timeout 2" << std::endl; return; }

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
}

void RevelesIO::TriggerTimeOfFlight()
{
    // TODO: Do we need a trigger?
}

MagDirection RevelesIO::ReadMagnetometer()
{
    if(!MagAvailable) { return new MagDirection{0.0f, 0.0f, 0.0f}; }

    return agm->ReadMag();
}

AccelDirection RevelesIO::ReadAccelerometer()
{
    if(!XGAvailable) { return new AccelDirection {0.0f, 0.0f, 0.0f}; }

    return agm->ReadAccel();
}

GyroDirection RevelesIO::ReadGyroscope()
{
    if(!XGAvailable) { return new GyroDirection {0.0f, 0.0f, 0.0f}; }

    return agm->ReadGyro();
}
