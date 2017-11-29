#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

class GPIO
{
public:
    GPIO();


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
