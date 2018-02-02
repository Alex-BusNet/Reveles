#include "lsm9ds1.h"
#include <wiringPiI2C.h>

LSM9DS1::LSM9DS1()
{
    hasMag = false;
    hasXG = false;
}

bool LSM9DS1::AccelGyroFound()
{
    return hasXG;
}

bool LSM9DS1::MagFound()
{
    return hasMag;
}

void LSM9DS1::setup()
{
    wiringPiI2CSetup(MAG_ADDR);
    wiringPiI2CSetup(XG_ADDR);

    uint8_t id = wiringPiI2CReadReg8(XG_ADDR, WHO_AM_I);

    if (id == XG_ID)
        hasXG = true;

    id = wiringPiI2CReadReg8(MAG_ADDR, WHO_AM_I);

    if( id == MAG_ID)
        hasMag = true;

    setupMag(MAG_4GAUSS);
    setupAccel(XL_8G);
    setupGyro(GYRO_SCALE_245DPS);
}

void LSM9DS1::setupMag(LSM9DS1::MagGain gain)
{
    uint8_t reg = wiringPiI2CReadReg8(MAG_ADDR, CTRL_REG2_M);
    reg &= ~(0b01100000);
    reg |= gain;
    wiringPiI2CWriteReg8(MAG_ADDR, CTRL_REG2_M, reg);

    switch(gain)
    {
    case MAG_4GAUSS:
        magLSB = MAG_MGAUSS_4GAUSS;
        break;
    case MAG_8GAUSS:
        magLSB = MAG_MGAUSS_8GAUSS;
        break;
    case MAG_12GAUSS:
        magLSB = MAG_MGAUSS_12GAUSS;
        break;
    case MAG_16GAUSS:
        magLSB = MAG_MGAUSS_16GAUSS;
        break;
    }
}

void LSM9DS1::setupAccel(LSM9DS1::AccelRange range)
{
    uint8_t reg = wiringPiI2CReadReg8(XG_ADDR, CTRL_REG6_XL);
    reg &= ~(0b00011000);
    reg |= range;
    wiringPiI2CWriteReg8(XG_ADDR, CTRL_REG6_XL, reg);

    switch(range)
    {
    case XL_2G:
        accelLSB = ACCEL_MG_LSB_2G;
        break;
    case XL_4G:
        accelLSB = ACCEL_MG_LSB_4G;
        break;
    case XL_8G:
        accelLSB = ACCEL_MG_LSB_8G;
        break;
    case XL_16G:
        accelLSB = ACCEL_MG_LSB_16G;
        break;
    }
}

void LSM9DS1::setupGyro(LSM9DS1::GyroScale scale)
{
    uint8_t reg = wiringPiI2CReadReg8(XG_ADDR, CTRL_REG1_G);
    reg &= ~(0b00110000);
    reg |= scale;
    wiringPiI2CWriteReg8(XG_ADDR, CTRL_REG1_G, reg);

    switch(scale)
    {
    case GYRO_SCALE_245DPS:
        gyroDPSDigit = GYRO_DPS_DIGIT_245DPS;
        break;
    case GYRO_SCALE_500DPS:
        gyroDPSDigit = GYRO_DPS_DIGIT_500DPS;
        break;
    case GYRO_SCALE_2000DPS:
        gyroDPSDigit = GYRO_DPS_DIGIT_2000DPS;
        break;
    }
}

/*
 * Reads the magnetometer and
 * returns the parsed result.
 *
 * This function assumes the magnetometer
 * exists.
 */
MagDirection LSM9DS1::ReadMag()
{
    uint8_t xlo = wiringPiI2CReadReg8(MAG_ADDR, M_OUT_X_L);
    int16_t xhi = wiringPiI2CReadReg8(MAG_ADDR, M_OUT_X_H);
    uint8_t ylo = wiringPiI2CReadReg8(MAG_ADDR, M_OUT_Y_L);
    int16_t yhi = wiringPiI2CReadReg8(MAG_ADDR, M_OUT_Y_H);
    uint8_t zlo = wiringPiI2CReadReg8(MAG_ADDR, M_OUT_Z_L);
    int16_t zhi = wiringPiI2CReadReg8(MAG_ADDR, M_OUT_Z_H);

    xhi <<= 8;
    xhi |= xlo;
    yhi <<= 8;
    yhi |= ylo;
    zhi <<= 8;
    zhi |= zlo;

    xhi *= magLSB;
    xhi /= 1000;

    yhi *= magLSB;
    yhi /= 1000;

    zhi *= magLSB;
    zhi /= 1000;

    return MagDirection{xhi, yhi, zhi};
}

AccelDirection LSM9DS1::ReadAccel()
{
    uint8_t xlo = wiringPiI2CReadReg8(XG_ADDR, XL_OUT_X_L);
    int16_t xhi = wiringPiI2CReadReg8(XG_ADDR, XL_OUT_X_H);
    uint8_t ylo = wiringPiI2CReadReg8(XG_ADDR, XL_OUT_Y_L);
    int16_t yhi = wiringPiI2CReadReg8(XG_ADDR, XL_OUT_Y_H);
    uint8_t zlo = wiringPiI2CReadReg8(XG_ADDR, XL_OUT_Z_L);
    int16_t zhi = wiringPiI2CReadReg8(XG_ADDR, XL_OUT_Z_H);

    xhi <<= 8;
    xhi |= xlo;
    yhi <<= 8;
    yhi |= ylo;
    zhi <<= 8;
    zhi |= zlo;

    xhi *= accelLSB;
    xhi /= 1000;
    xhi *= GRAVITY_STANDARD;

    yhi *= accelLSB;
    yhi /= 1000;
    yhi *= GRAVITY_STANDARD;

    zhi += accelLSB;
    zhi /= 1000;
    zhi *= GRAVITY_STANDARD;

    return AccelDirection{xhi, yhi, zhi};
}


GyroDirection LSM9DS1::ReadGyro()
{
    uint8_t xlo = wiringPiI2CReadReg8(XG_ADDR, GYRO_OUT_X_L);
    int16_t xhi = wiringPiI2CReadReg8(XG_ADDR, GYRO_OUT_X_H);
    uint8_t ylo = wiringPiI2CReadReg8(XG_ADDR, GYRO_OUT_Y_L);
    int16_t yhi = wiringPiI2CReadReg8(XG_ADDR, GYRO_OUT_Y_H);
    uint8_t zlo = wiringPiI2CReadReg8(XG_ADDR, GYRO_OUT_Z_L);
    int16_t zhi = wiringPiI2CReadReg8(XG_ADDR, GYRO_OUT_Z_H);

    xhi <<= 8;
    xhi |= xlo;
    yhi <<= 8;
    yhi |= ylo;
    zhi <<= 8;
    zhi |= zlo;

    xhi *= gyroDPSDigit;

    yhi *= gyroDPSDigit;

    zhi *= gyroDPSDigit;

    return GyroDirection{xhi, yhi, zhi};
}
