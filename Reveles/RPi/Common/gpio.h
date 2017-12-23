#ifndef GPIO_H
#define GPIO_H

#include <cstdint>
#include "datatypes.h"
#include "../../Libraries/wiringPi/wiringPi.h"

class GPIO
{
public:
    GPIO(int pin = 0, Mode m = OUT);

    void writeMotor();
    GPSCoord readGPS();
    int readMotor();

    int readUltrasonic(UltrasonicPos usp);

private:
    int pin;
    Mode mode;
};

class Uart : public GPIO
{
public:
    Uart(int txPin, int rxPin, int baudRate);
    void write(uint8_t devAddr, uint8_t regAddr);
    uint8_t readByte(uint8_t devAddr, uint8_t regAddr, int bytes);

private:
    int tx, rx, baud;
};

#endif // GPIO_H
