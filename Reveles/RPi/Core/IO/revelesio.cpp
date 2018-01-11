#include "revelesio.h"
#include <iostream>

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
    return GPSCoord();
}

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

    //===============================================
    // May need to thread this or place on interrupt.
    // This may start to slow down the GUI.
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
    //===============================================
}
