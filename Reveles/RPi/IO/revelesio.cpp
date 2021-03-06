﻿#include "revelesio.h"
#include <iostream>
#include <random>
#include <math.h>
#include "Common/logger.h"
#include "Common/messages.h"
#include <QtConcurrent>
#include <cstdlib>
#include <chrono>

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
    pinMode(SIG, INPUT);
//    pinMode(NUCLEO_ISR, OUTPUT);
    pinMode(TRIG, OUTPUT);

    arduinoFound = false;

    fdArduino = wiringPiI2CSetup(ARDUINO);
    fdNucleo[0] = -1; //wiringPiI2CSetup(NUCLEO_FRONT);
    fdNucleo[1] = -1; //wiringPiI2CSetup(NUCLEO_REAR);

    wiringPiI2CWrite(fdArduino, COM_CHECK);
    uint8_t res = CMD_FLUSH;
    chrono::steady_clock::time_point waitTime = chrono::steady_clock::now();
    chrono::steady_clock::time_point::rep elapsedTime;

    while (true)
    {
        res = wiringPiI2CRead(fdArduino);
        if(res == COM_CHECK) { break; }
        elapsedTime = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - waitTime).count();
        // One second timeout;
        if(elapsedTime > 2000) { break; }
    }

    Logger::writeLine(instance(), QString("Arduino response: 0x%1").arg(res, 2, HEX, QChar('0')));
    Logger::writeLine(instance(), QString("Arduino Response time: %1 ms").arg(elapsedTime));

    waitTime = chrono::steady_clock::now();

    arduinoFound = (res == COM_CHECK);

    emit arduinoStat(arduinoFound);

    Logger::writeLine(instance(), QString("Initializing Time of Flight sensors..."));
    InitToFSensors();

    fdToF[0] = tofInit(1, TOF_F_LEFT, 1);
    fdToF[1] = -1; //tofInit(1, TOF_F_CENTER, 1);
    fdToF[2] = tofInit(1, TOF_F_RIGHT, 1);
    fdToF[3] = tofInit(1, TOF_RIGHT, 1);
    fdToF[4] = tofInit(1, TOF_R_RIGHT, 1);
    fdToF[5] = -1; //tofInit(1, TOF_R_CENTER, 1);
    fdToF[6] = tofInit(1, TOF_R_LEFT, 1);
    fdToF[7] = tofInit(1, TOF_LEFT, 1);

//    system("i2cdetect -y 1");

    res = CMD_FLUSH;

    Logger::writeLine(instance(), Reveles::ARDUINO_FOUND.arg(ARDUINO, 2, 16, QChar('0')).arg(B2STR(arduinoFound)));
//    Logger::writeLine(instance(), Reveles::NUCLEO_FOUND.arg(NUCLEO_FRONT, 2, 16, QChar('0')).arg((nucleoFound[0]) ? "True":"False"));
//    Logger::writeLine(instance(), Reveles::NUCLEO_FOUND.arg(NUCLEO_REAR, 2, 16, QChar('0')).arg((nucleoFound[1]) ? "True":"False"));

    int *m = new int(1), *r = new int(1);
    for(int i = 0; i < 8; i++)
    {
        Logger::writeLine(instance(), QString("fdToF[%1]: %2").arg(i).arg(fdToF[i]));
        tofDist[i] = -1;

        if(fdToF[i] != -1)
        {
            tofGetModel(m, r, fdToF[i]);
            Logger::writeLine(instance(), QString("[%1] %2\t%3").arg(i).arg(*m).arg(*r));
        }
    }

    lastKnownCoord = GPSCoord{0, 0};

    agm = new LSM9DS1();
    agm->setup();

    Logger::writeLine(instance(), Reveles::FILE_DESCRIPTORS.arg(agm->XGFD()).arg(agm->MagFD()).arg(fdArduino).arg(fdNucleo[0]).arg(fdNucleo[1]));

    XGAvailable = agm->AccelGyroFound();
    MagAvailable = agm->MagFound();

    stopParser = false;
    stopToF = false;
    dist = 0.0f;
    inch = 156;
    angle = 90;
    motorDir = M_FWD;
	servoDir = RET_NEUTRAL;

    parser = QtConcurrent::run([=]()
    {
        ParseQueue();
    });
}

void RevelesIO::EnqueueRequest(RIOData riod)
{
    ioRequestQueue.enqueue(riod);
}

void RevelesIO::StartNav()
{
    if(stopToF) stopToF = false;

    tofReader = QtConcurrent::run([=]()
    {
        float distMM = 0.0f;
        while(!stopToF)
        {
            for(int i = 0; i < 8; i++)
            {
                if(fdToF[i] != -1)
                {
                    distMM = tofReadDistance(fdToF[i]);
                    tofDist[i] = ((float)distMM / 25.4f); // mm to inches
                    if(tofDist[i] > 66.0f)
                    {
                        tofDist[i] = 66.0f; /* inches */
                    }

                    emit tofReady(i, tofDist[i]);
                    distMM = 0.0f;
                }
            }

            delay(49);
        }
    });
}

void RevelesIO::StopNav()
{
    Logger::writeLine(instance(), QString("Ending Navigation..."));
    SetMotorDirection(M_STOP);

    stopToF = true;
    if(tofReader.isRunning())
        tofReader.cancel();

    ioRequestQueue.clear();

    Logger::writeLine(instance(), QString("Done."));
}

void RevelesIO::CloseIO()
{
    Logger::writeLine(instance(), QString("Closing IO..."));
    stopParser = true;
    stopToF = true;

    Logger::writeLine(instance(), QString("Exiting IO Queue parser.."));
    if(parser.isRunning())
        parser.cancel();

    Logger::writeLine(instance(), QString("Exiting ToF reader..."));
    if(tofReader.isRunning())
        tofReader.cancel();

//    for(int i = 0; i < 8; i++)
//    {
//        if(fdToF[i] != -1)
//        {
//            closeToF(fdToF[i]);
//            fdToF[i] = -1;
//        }
//    }

    Logger::writeLine(instance(), QString("Disconnecting Arduino..."));
    wiringPiI2CWrite(fdArduino, END);
    delay(I2C_TRANSMIT_DELAY);

    Logger::writeLine(instance(), QString("Done."));
}

RevelesIO::RevelesIO()
{

}

RevelesIO::~RevelesIO()
{
    CloseIO();
    if(agm != NULL)
        delete agm;
}

//================================================================================
//                      I2C Command Structure (RPi -> Arduino):
// <START> -> <Function>|-> <US reading> -> <Direction> -> <ToF reading> -> <END>
//                      |-> <Servo Direction> -> <Servo value> -> <END>
//                      |-> <ToF shutdown pin num> -> <END>
//                      |-> <END> (GPS Command)
//--------------------------------------------------------------------------------
//          [ ALL COMMANDS ARE SENT IN HEXADECIMAL, VALUES ARE IN DECIMAL ]
//
// Possible values
//    Function:
//      CMD_M - RPi is sending drive instructions.
//      CMD_G - RPi is requesting GPS coordinates.
//      CMD_S - RPi is sending turning instructions.
//      CMD_P - RPi is sending which ToF Shutdown pin to release
//    US Value: Floating Point value for PWM signal.
//      (Motor command only)
//      Value is in inches.
//      Value is not adjusted to fit between 255 and 0.
//    Direction: (Motor command only)
//      M_FWD  - Motors should drive forwards.
//      M_STOP - Motors should stop driving.
//      M_REV  - Motors should drive in reverse.
//    ToF value: Floating point value from Time of Flight sensors.
//      (Motor command only)
//      Value is in inches.
//    ToF Shutdown pin: Integer indicating sensor to restart.
//    Servo Direction: (Servo command only)
//      TURN_RIGHT - Indicates Reveles should turn to the right. (Relative to self).
//      TURN_LEFT  - Indicates Reveles shoudl turn to the left. (Relative to self).
//    Servo Value: (Servo command only)
//      Degrees servo should turn.
//      Value is always positive.
//      Value is not mapped to PWM output.
//================================================================================

void RevelesIO::SendMotorUpdate()
{
    // Total time to transmit: 510ms
    wiringPiI2CWrite(fdArduino, START);
    delay(I2C_TRANSMIT_DELAY);
    wiringPiI2CWrite(fdArduino, CMD_M);
    delay(I2C_TRANSMIT_DELAY);
    wiringPiI2CWrite(fdArduino, inch);
    delay(I2C_TRANSMIT_DELAY);
    wiringPiI2CWrite(fdArduino, motorDir);
    delay(I2C_TRANSMIT_DELAY);
    /// NOTE: We need to update this part based on travel direction.
    wiringPiI2CWrite(fdArduino, tofDist[0]);
    delay(I2C_TRANSMIT_DELAY);
    wiringPiI2CWrite(fdArduino, END);
    delay(I2C_TRANSMIT_DELAY);

//    Logger::writeLine(instance(), QString("START:         0x%1").arg(START, 2, 16, QChar('0')));
//    Logger::writeLine(instance(), QString("Motor Command: 0x%1").arg(CMD_M, 2, 16, QChar('0')));
    Logger::writeLine(instance(), QString("US Dist:       %1 in").arg(inch, 4, 10, QChar('0')));
    Logger::writeLine(instance(), QString("Motor Dir:     0x%1").arg(motorDir, 2, 16, QChar('0')));
//    Logger::writeLine(instance(), QString("ToF Dist:      %1 in").arg(tofDist[1], 4, 10, QChar('0')));
//    Logger::writeLine(instance(), QString("END:           0x%1").arg(END, 2, 16, QChar('0')));
}

void RevelesIO::SetMotorDirection(uint8_t dir)
{
    motorDir = dir;
    SendMotorUpdate();
    emit motorDirectionUpdate(dir);
}

void RevelesIO::SendGPSRequest()
{
    wiringPiI2CWrite(fdArduino, START);
    delay(I2C_TRANSMIT_DELAY);
    wiringPiI2CWrite(fdArduino, CMD_G);
    delay(I2C_TRANSMIT_DELAY);
    wiringPiI2CWrite(fdArduino, END);
    delay(I2C_TRANSMIT_DELAY);

//    Logger::writeLine(instance(), Reveles::I2C_GPS_SEND.arg(START, 2, 16, QChar('0')));
//    Logger::writeLine(instance(), Reveles::I2C_GPS_SEND.arg(CMD_G, 2, 16, QChar('0')));
//    Logger::writeLine(instance(), Reveles::I2C_GPS_SEND.arg(END, 2, 16, QChar('0')));
}

void RevelesIO::SendServoUpdate()
{
    // Total time to transmit: 425ms
	wiringPiI2CWrite(fdArduino, START);
    delay(I2C_TRANSMIT_DELAY);
	wiringPiI2CWrite(fdArduino, CMD_S);
    delay(I2C_TRANSMIT_DELAY);
	wiringPiI2CWrite(fdArduino, servoDir);
    delay(I2C_TRANSMIT_DELAY);
	
	if(servoDir == TURN_LEFT)
        angle = 180;
	else if(servoDir == TURN_RIGHT)
        angle = 0;
	else
        angle = 90;

    wiringPiI2CWrite(fdArduino, angle);
    delay(I2C_TRANSMIT_DELAY);
	wiringPiI2CWrite(fdArduino, END);
    delay(I2C_TRANSMIT_DELAY);

    emit servoStatus(true, servoDir);
    emit servoStatus(false, servoDir);

//    Logger::writeLine(instance(), Reveles::I2C_SERVO_SEND.arg(START, 2, 16, QChar('0')));
//    Logger::writeLine(instance(), Reveles::I2C_SERVO_SEND.arg(CMD_S, 2, 16, QChar('0')));
//    Logger::writeLine(instance(), Reveles::I2C_SERVO_SEND.arg(servoDir, 2, 16, QChar('0')));
//    Logger::writeLine(instance(), Reveles::I2C_SERVO_ANGLE.arg(angle, 3, 10, QChar('0')));
//    Logger::writeLine(instance(), Reveles::I2C_SERVO_SEND.arg(END, 2, 16, QChar('0')));
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
    uint8_t response = wiringPiI2CRead(fdArduino);

    if(response == START)
    {
        bool readLat = false, readLong = false;
        while(response != END)
        {
            response = wiringPiI2CRead(fdArduino);
            Logger::writeLine(instance(), QString("Arduino Response: %1").arg(response));

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
    int idx = 0;
    if(sel == US_FRONT)
    {
        idx = 0;
        digitalWrite(SEL_A, LOW); // Get the A select bit. This should already by in index 0
        digitalWrite(SEL_B, LOW); // Get the B select bit and shift into index 0
    }
    else if(sel == US_FRONT_STAIR)
    {
        idx = 1;
        digitalWrite(SEL_A, HIGH);
        digitalWrite(SEL_B, LOW);
    }
    else if(sel == US_BACK)
    {
        idx = 2;
        digitalWrite(SEL_A, LOW);
        digitalWrite(SEL_B, HIGH);
    }
    else if(sel == US_BACK_STAIR)
    {
        idx = 3;
        digitalWrite(SEL_A, HIGH);
        digitalWrite(SEL_B, HIGH);
    }

    digitalWrite(TRIG, LOW);
    delay(50);

    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    unsigned long ping, pong, trigStart;

    trigStart = micros();

    while(digitalRead(ECHO) == LOW && ((micros() - trigStart) < TIMEOUT)){;}

    if(((micros() - trigStart) > TIMEOUT)) { emit usReady(idx, -1); return -1;}

    ping = micros();

    while(digitalRead(ECHO) == HIGH && ((micros() - trigStart) < TIMEOUT)) {;}

    if(((micros() - trigStart) > TIMEOUT)) { emit usReady(idx, -1); return -1; }

    pong = micros();

    dist = (float)(pong - ping) * 0.017150; // Convert to cm
    dist /= 2.54f; // Convert to inches

    if(dist > 171.0f)
        dist = 171.0f;

    // Currently used for D-Bus comms back to GUI.
    emit usReady(idx, dist);

    return dist;
}

float RevelesIO::ReadTimeOfFlight(int sensorNum)
{
    return tofDist[sensorNum];
}

bool RevelesIO::readPIR(bool rear)
{
    if(rear)
    {
        digitalWrite(SEL_A, LOW);
        digitalWrite(SEL_B, HIGH);
    }
    else
    {
        digitalWrite(SEL_A, LOW);
        digitalWrite(SEL_B, LOW);
    }

    int val = digitalRead(SIG);
    emit pirStat((val == 1) ? true : false, !rear);

    return ((val == 1) ? true : false);
}

MagDirection RevelesIO::ReadMagnetometer()
{
    if(!MagAvailable) { return MagDirection{0.0, 0.0, 0.0}; }

    return agm->ReadMag();
}

AccelDirection RevelesIO::ReadAccelerometer()
{
    if(!XGAvailable) { return AccelDirection{0.0, 0.0, 0.0}; }

    return agm->ReadAccel();
}

GyroDirection RevelesIO::ReadGyroscope()
{
    if(!XGAvailable) { return GyroDirection{0.0, 0.0, 0.0}; }

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

void RevelesIO::ParseQueue()
{
    while(!stopParser)
    {
        if(!ioRequestQueue.isEmpty())
        {
            RIOData riod = ioRequestQueue.dequeue();
            Logger::writeLine(instance(), QString("IO Request found: %1").arg(riod.cmd));

            if(riod.cmd == IO_MOTOR)
            {
                inch = riod.specData;
                SetMotorDirection(riod.data);
            }
            else if (riod.cmd == IO_GPS)
            {
                SendGPSRequest();
            }
            else if(riod.cmd == IO_SERVO)
            {
                angle = riod.specData;
                SetServoDirection(riod.data);
            }
        }

        delay(100);
    }

    SetMotorDirection(M_STOP);
}

void RevelesIO::SendToFRequest(int sensorNum)
{
//    if(nucleoFound[0] && sensorNum < 4)
//    {
//        wiringPiI2CWrite(fdNucleo[0], START);
//        delay(I2C_TRANSMIT_DELAY);
//        wiringPiI2CWrite(fdNucleo[0], CMD_T);
//        delay(I2C_TRANSMIT_DELAY);
//        wiringPiI2CWrite(fdNucleo[0], sensorNum);
//        delay(I2C_TRANSMIT_DELAY);
//        wiringPiI2CWrite(fdNucleo[0], END);
//        delay(I2C_TRANSMIT_DELAY);

//        tofDist[sensorNum] = wiringPiI2CRead(fdNucleo[0]);
//        delay(I2C_TRANSMIT_DELAY);
//        delay(I2C_TRANSMIT_DELAY);
//        Logger::writeLine(instance(), Reveles::TOF_I2C_RESPONSE.arg(NUCLEO_FRONT).arg(tofDist[sensorNum]));
//    }
//    else if(nucleoFound[1] && sensorNum > 4)
//    {
//        wiringPiI2CWrite(fdNucleo[1], START);
//        delay(I2C_TRANSMIT_DELAY);
//        wiringPiI2CWrite(fdNucleo[1], CMD_T);
//        delay(I2C_TRANSMIT_DELAY);
//        wiringPiI2CWrite(fdNucleo[1], (sensorNum - 4));
//        delay(I2C_TRANSMIT_DELAY);
//        wiringPiI2CWrite(fdNucleo[1], END);
//        delay(I2C_TRANSMIT_DELAY);

//        tofDist[sensorNum] = wiringPiI2CRead(fdNucleo[1]);
//        delay(I2C_TRANSMIT_DELAY);
//        delay(I2C_TRANSMIT_DELAY);
//        Logger::writeLine(instance(), Reveles::TOF_I2C_RESPONSE.arg(NUCLEO_REAR).arg(tofDist[sensorNum]));
//    }

    //    emit tofReady(sensorNum, tofDist[sensorNum]);
}

/**
 * @brief Initializes the Time of Flight sensors with their new I2C addresses
 */
void RevelesIO::InitToFSensors()
{
    if(arduinoFound)
    {
        int defFd = wiringPiI2CSetup(TOF_DEFAULT_ADDR);

        for(int i = 0; i < 8; i++)
        {
            // Currently, the Front and Rear center ToFs are not being used.
            if(ToFNewAddr[i] == TOF_F_CENTER || ToFNewAddr[i] == TOF_R_CENTER)
                continue;

            wiringPiI2CWrite(fdArduino, START);
            delay(I2C_TRANSMIT_DELAY);
            wiringPiI2CWrite(fdArduino, CMD_P);
            delay(I2C_TRANSMIT_DELAY);
            wiringPiI2CWrite(fdArduino, i);
            delay(I2C_TRANSMIT_DELAY);
            wiringPiI2CWrite(fdArduino, END);
            delay(I2C_TRANSMIT_DELAY);

            delay(100);

            wiringPiI2CWriteReg8(defFd, TOF_I2C_ADDR_REG, ToFNewAddr[i]);
            delay(I2C_TRANSMIT_DELAY);

            delay(100);
        }
    }
}
