#include "revelesio.h"
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
    pinMode(NUCLEO_ISR, OUTPUT);
    pinMode(TRIG, OUTPUT);

    arduinoFound = false;

    agm = new LSM9DS1();
    agm->setup();

    fdArduino = wiringPiI2CSetup(ARDUINO);
    fdNucleo[0] = wiringPiI2CSetup(NUCLEO_FRONT);
    fdNucleo[1] = wiringPiI2CSetup(NUCLEO_REAR);

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

    res = CMD_FLUSH;

    while(true)
    {
        res = wiringPiI2CRead(fdNucleo[0]);
        if(res == COM_CHECK) { break; }
        elapsedTime = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - waitTime).count();
        if(elapsedTime > 2000) { break; }
    }

    if(res != COM_CHECK)
        cout << "Error at " << NUCLEO_FRONT << ": " << errno << " " << strerror(errno) <<  endl;

    emit nucleoStat(res == COM_CHECK, 0);
    nucleoFound[0] = (res == COM_CHECK);
    res = CMD_FLUSH;

    waitTime = chrono::steady_clock::now();

    while(true)
    {
        res = wiringPiI2CRead(fdNucleo[1]);
        if(res == COM_CHECK) { break; }
        elapsedTime = chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - waitTime).count();
        if(elapsedTime > 2000) { break; }
    }

    if(fdNucleo[1] == -1)
        cout << "Error at " << NUCLEO_REAR << ": " << errno << " " << strerror(errno) <<  endl;

    emit nucleoStat(res == COM_CHECK, 1);
    //This is temporary.
    nucleoFound[0] = true;//(res == COM_CHECK);

    Logger::writeLine(instance(), Reveles::ARDUINO_FOUND.arg(ARDUINO, 2, 16, QChar('0')).arg(B2STR(arduinoFound)));
    Logger::writeLine(instance(), Reveles::NUCLEO_FOUND.arg(NUCLEO_FRONT, 2, 16, QChar('0')).arg((nucleoFound[0]) ? "True":"False"));
    Logger::writeLine(instance(), Reveles::NUCLEO_FOUND.arg(NUCLEO_REAR, 2, 16, QChar('0')).arg((nucleoFound[1]) ? "True":"False"));
    Logger::writeLine(instance(), Reveles::FILE_DESCRIPTORS.arg(agm->XGFD()).arg(agm->MagFD()).arg(fdArduino).arg(fdNucleo[0]).arg(fdNucleo[1]));

    for(int i = 0; i < 8; i++)
    {
        tofDist[i] = 66;
    }

    lastKnownCoord = GPSCoord{0, 0};

    XGAvailable = agm->AccelGyroFound();
    MagAvailable = agm->MagFound();

    stopParser = false;
    stopToF = false;
    dist = 0;
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
        while(!stopToF)
        {
            for(int i = 0; i < 8; i++)
            {
                Logger::writeLine(instance(), QString("Sending ToF Request for %1").arg(i));
                ReadTimeOfFlight(i);
            }

            delay(500);
        }
    });
}

void RevelesIO::StopNav()
{
    Logger::writeLine(instance(), QString("Ending Navigation..."));

    stopToF = true;
    if(tofReader.isRunning())
        tofReader.waitForFinished();

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
//                      |-> <END> (GPS Command)
//--------------------------------------------------------------------------------
//          [ ALL COMMANDS ARE SENT IN HEXADECIMAL, VALUES ARE IN DECIMAL ]
//
// Possible values
//    Function:
//      CMD_M - RPi is sending drive instructions.
//      CMD_G - RPi is requesting GPS coordinates.
//      CMD_S - RPi is sending turning instructions.
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
//    Servo Direction: (Servo command only)
//      TURN_RIGHT - Indicates Reveles should turn to the right. (Relative to self).
//      TURN_LEFT  - Indicates Reveles shoudl turn to the left. (Relative to self).
//    Servo Value: (Servo command only)
//      Degrees servo should turn.
//      Value is always positive.
//      Value is not mapped to PWM output.
//================================================================================

//================================================================================
//                      I2C Command Structure (RPi -> Nucleo):
//                  <START> -> <Function> -> <ToF number> -> <END>
//--------------------------------------------------------------------------------
//                     [ ALL COMMANDS ARE SENT IN HEXADECIMAL ]
// Possible values
//    Function:
//      CMD_T - RPi is requesting Time of Flight Sensor data.
//    ToF number: (ToF command only)
//      Index or I2C address of desired ToF sensor.
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
    wiringPiI2CWrite(fdArduino, tofDist[1]);
    delay(I2C_TRANSMIT_DELAY);
    wiringPiI2CWrite(fdArduino, END);
    delay(I2C_TRANSMIT_DELAY);

//    Logger::writeLine(instance(), QString("START:         0x%1").arg(START, 2, 16, QChar('0')));
//    Logger::writeLine(instance(), QString("Motor Command: 0x%1").arg(CMD_M, 2, 16, QChar('0')));
//    Logger::writeLine(instance(), QString("US Dist:       %1 in").arg(inch, 4, 10, QChar('0')));
//    Logger::writeLine(instance(), QString("Motor Dir:     0x%1").arg(motorDir, 2, 16, QChar('0')));
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
int RevelesIO::triggerUltrasonic(uint8_t sel)
{
    // Check this, I don't believe it
    // works the way I think it does. -Alex 1/21/18
    if(sel == US_FRONT)
    {
        digitalWrite(SEL_A, HIGH);         // Get the A select bit. This should already by in index 0
        digitalWrite(SEL_B, LOW);// >> 1); // Get the B select bit and shift into index 0
    }
    else if(sel == US_RIGHT)
    {
        digitalWrite(SEL_A, LOW);
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

int RevelesIO::ReadTimeOfFlight(int sensorNum)
{
    return tofDist[sensorNum];
}

bool RevelesIO::readPIR(bool rear)
{
    if(rear)
    {
        digitalWrite(SEL_A, LOW);  // Get the A select bit. This should already by in index 0
        digitalWrite(SEL_B, HIGH); // Get the B select bit and shift into index 0
    }
    else
    {
        digitalWrite(SEL_A, LOW);
        digitalWrite(SEL_B, LOW);
    }

    int val = digitalRead(SIG);
    emit pirStat(val ? true : false);

    return val ? true : false;
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
            else if(riod.cmd == IO_TOF)
            {
                ReadTimeOfFlight(riod.data);
            }
        }

        delay(100);
    }

    SetMotorDirection(M_STOP);
}

void RevelesIO::SendToFRequest(int sensorNum)
{
    if(nucleoFound[0] && sensorNum < 4)
    {
        wiringPiI2CWrite(fdNucleo[0], START);
        delay(I2C_TRANSMIT_DELAY);
        wiringPiI2CWrite(fdNucleo[0], CMD_T);
        delay(I2C_TRANSMIT_DELAY);
        wiringPiI2CWrite(fdNucleo[0], sensorNum);
        delay(I2C_TRANSMIT_DELAY);
        wiringPiI2CWrite(fdNucleo[0], END);
        delay(I2C_TRANSMIT_DELAY);

        tofDist[sensorNum] = wiringPiI2CRead(fdNucleo[0]);
        delay(I2C_TRANSMIT_DELAY);
        Logger::writeLine(instance(), Reveles::TOF_I2C_RESPONSE.arg(NUCLEO_FRONT).arg(tofDist[sensorNum]));
    }
    else if(nucleoFound[1] && sensorNum > 4)
    {
        wiringPiI2CWrite(fdNucleo[1], START);
        delay(I2C_TRANSMIT_DELAY);
        wiringPiI2CWrite(fdNucleo[1], CMD_T);
        delay(I2C_TRANSMIT_DELAY);
        wiringPiI2CWrite(fdNucleo[1], (sensorNum - 4));
        delay(I2C_TRANSMIT_DELAY);
        wiringPiI2CWrite(fdNucleo[1], END);
        delay(I2C_TRANSMIT_DELAY);

        tofDist[sensorNum] = wiringPiI2CRead(fdNucleo[1]);
        delay(I2C_TRANSMIT_DELAY);
        Logger::writeLine(instance(), Reveles::TOF_I2C_RESPONSE.arg(NUCLEO_REAR).arg(tofDist[sensorNum]));
    }
}
