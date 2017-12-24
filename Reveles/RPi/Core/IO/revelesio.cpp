#include "revelesio.h"

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

    wiringPiI2CSetup(MAG_ADDR);
    wiringPiI2CSetup(XG_ADDR);
    wiringPiI2CSetup(ARDUINO);
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
    return GPSCoord();
}

int RevelesIO::readSensor(uint8_t sel, SensorType type)
{
    digitalWrite(SEL_A, 0b001 & sel);
    digitalWrite(SEL_B, 0b010 & sel);
    digitalWrite(SEL_C, 0b100 & sel);

    if(type == US)
    {
        triggerUltrasonic();
        return digitalRead(ECHO);
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

void RevelesIO::triggerUltrasonic()
{
    digitalWrite(TRIG, HIGH);
}
