#include "gpio.h"

GPIO::GPIO(int pin, Mode m)
{
    this->pin = pin;
    this->mode = mode;
}


Uart::Uart(int txPin, int rxPin, int baudRate)
{
    this->tx = txPin;
    this->rx = rxPin;
    this->baud = baudRate;
}

void Uart::write(uint8_t devAddr, uint8_t regAddr)
{
    return;
}

uint8_t Uart::readByte(uint8_t devAddr, uint8_t regAddr, int bytes)
{
    return 0;
}
