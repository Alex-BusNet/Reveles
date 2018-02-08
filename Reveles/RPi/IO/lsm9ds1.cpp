#include "lsm9ds1.h"
#include <wiringPiI2C.h>
#include <wiringPi.h>

#include "Common/logger.h"
#include "Common/messages.h"

#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

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
    // Setup the I2C line for the Accel/Gyro.
    fdXG = wiringPiI2CSetup(XG_ADDR);

    // Setup the I2C line for the Magnetometer.
    fdMag = wiringPiI2CSetup(MAG_ADDR);

    wiringPiI2CWriteReg8(fdXG, CTRL_REG_8, 0x05);
    wiringPiI2CWriteReg8(fdMag, CTRL_REG2_M, 0x0C);

    delayMicroseconds(10);

    int id = wiringPiI2CReadReg8(fdXG, WHO_AM_I);
    Logger::writeLine(RevelesIO::instance(), Reveles::XG_ID_STR.arg(id, 2, 16));

    if(id == XG_ID)
        hasXG = true;

    id = wiringPiI2CReadReg8(fdMag, WHO_AM_I);
    Logger::writeLine(RevelesIO::instance(), Reveles::MAG_ID_STR.arg(id, 2, 16));

    if(id == MAG_ID)
        hasMag = true;

    // Gyro continuous
    wiringPiI2CWriteReg8(fdXG, CTRL_REG1_G, 0xC0);
    // Accel Continuous
    wiringPiI2CWriteReg8(fdXG, CTRL_REG5_XL, 0x38);
    // Accel 1KHz out data rate, BW set by ODR, 408Hz anti-aliasing
    wiringPiI2CWriteReg8(fdXG, CTRL_REG6_XL, 0xC0);

    // Mag continuous
    wiringPiI2CWriteReg8(fdMag, CTRL_REG3_M, 0x00);

    if(hasMag)
        setupMag(MAG_4GAUSS);

    if(hasXG)
    {
        setupAccel(XL_8G);
        setupGyro(GYRO_SCALE_245DPS);
    }
}

void LSM9DS1::setupMag(LSM9DS1::MagGain gain)
{
    uint8_t reg = wiringPiI2CReadReg8(fdMag, CTRL_REG2_M);
    reg &= ~(0b01100000);
    reg |= gain;
    wiringPiI2CWriteReg8(fdMag, CTRL_REG2_M, reg);

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
    uint8_t reg = wiringPiI2CReadReg8(fdXG, CTRL_REG6_XL);
    reg &= ~(0b00011000);
    reg |= range;
    wiringPiI2CWriteReg8(fdXG, CTRL_REG6_XL, reg);

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
    uint8_t reg = wiringPiI2CReadReg8(fdXG, CTRL_REG1_G);
    reg &= ~(0b00110000);
    reg |= scale;
    wiringPiI2CWriteReg8(fdXG, CTRL_REG1_G, reg);

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
    uint8_t xlo = wiringPiI2CReadReg8(fdMag, M_OUT_X_L);
    int16_t xhi = wiringPiI2CReadReg8(fdMag, M_OUT_X_H);
    uint8_t ylo = wiringPiI2CReadReg8(fdMag, M_OUT_Y_L);
    int16_t yhi = wiringPiI2CReadReg8(fdMag, M_OUT_Y_H);
    uint8_t zlo = wiringPiI2CReadReg8(fdMag, M_OUT_Z_L);
    int16_t zhi = wiringPiI2CReadReg8(fdMag, M_OUT_Z_H);

    xhi <<= 8;
    xhi |= xlo;
    yhi <<= 8;
    yhi |= ylo;
    zhi <<= 8;
    zhi |= zlo;

    MagDirection md;
    md.x  = xhi * magLSB;
    md.x /= 10;

    md.y  = yhi * magLSB;
    md.y /= 10;

    md.z  = zhi * magLSB;
    md.z /= 10;

    return md;
}

AccelDirection LSM9DS1::ReadAccel()
{
    uint8_t xlo = wiringPiI2CReadReg8(fdXG, XL_OUT_X_L);
    int16_t xhi = wiringPiI2CReadReg8(fdXG, XL_OUT_X_H);
    uint8_t ylo = wiringPiI2CReadReg8(fdXG, XL_OUT_Y_L);
    int16_t yhi = wiringPiI2CReadReg8(fdXG, XL_OUT_Y_H);
    uint8_t zlo = wiringPiI2CReadReg8(fdXG, XL_OUT_Z_L);
    int16_t zhi = wiringPiI2CReadReg8(fdXG, XL_OUT_Z_H);

    xhi <<= 8;
    xhi |= xlo;
    yhi <<= 8;
    yhi |= ylo;
    zhi <<= 8;
    zhi |= zlo;

    AccelDirection ad;

    ad.x  = xhi * accelLSB;
    ad.x /= 1000;
    ad.x *= GRAVITY_STANDARD;

    ad.y  = yhi * accelLSB;
    ad.y /= 1000;
    ad.y *= GRAVITY_STANDARD;

    ad.z  = zhi * accelLSB;
    ad.z /= 1000;
    ad.z *= GRAVITY_STANDARD;

    return ad;
}


GyroDirection LSM9DS1::ReadGyro()
{
    uint8_t xlo = wiringPiI2CReadReg8(fdXG, GYRO_OUT_X_L);
    int16_t xhi = wiringPiI2CReadReg8(fdXG, GYRO_OUT_X_H);
    uint8_t ylo = wiringPiI2CReadReg8(fdXG, GYRO_OUT_Y_L);
    int16_t yhi = wiringPiI2CReadReg8(fdXG, GYRO_OUT_Y_H);
    uint8_t zlo = wiringPiI2CReadReg8(fdXG, GYRO_OUT_Z_L);
    int16_t zhi = wiringPiI2CReadReg8(fdXG, GYRO_OUT_Z_H);

    xhi <<= 8;
    xhi |= xlo;
    yhi <<= 8;
    yhi |= ylo;
    zhi <<= 8;
    zhi |= zlo;

    GyroDirection gd;
    gd.x = xhi * gyroDPSDigit;

    gd.y = yhi * gyroDPSDigit;

    gd.z = zhi * gyroDPSDigit;

    return gd;
}
