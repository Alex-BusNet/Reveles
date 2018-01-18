#ifndef LSM9DS1_H
#define LSM9DS1_H

/*
 * The enumerations and defines are from the
 * datasheet for the LSM9DS1 (see Reveles project binder)
 * and the Adafruit Arduino library for the LSM9DS1.
 */

#include <stdint.h>
#include "Common/datatypes.h"

#define  XG_ADDR  0x6B
#define MAG_ADDR  0x1E

#define    XG_ID  0b01101000
#define   MAG_ID  0b00111101

#define ACCEL_MG_LSB_2G    0.061f
#define ACCEL_MG_LSB_4G    0.122f
#define ACCEL_MG_LSB_8G    0.244f
#define ACCEL_MG_LSB_16G   0.732f

#define MAG_MGAUSS_4GAUSS    0.14f
#define MAG_MGAUSS_8GAUSS    0.29f
#define MAG_MGAUSS_12GAUSS   0.43f
#define MAG_MGAUSS_16GAUSS   0.58f

#define GYRO_DPS_DIGIT_245DPS   0.00875f
#define GYRO_DPS_DIGIT_500DPS   0.01750f
#define GYRO_DPS_DIGIT_2000DPS  0.07000f

#define GRAVITY_STANDARD        9.80665f

class LSM9DS1
{
public:
    LSM9DS1();

    void setup();

    GyroDirection ReadGyro();
    AccelDirection ReadAccel();
    MagDirection ReadMag();
    // read temp(?)

    bool AccelGyroFound();
    bool MagFound();

private:
    // Variables
    bool hasXG, hasMag;

    float magLSB;
    float accelLSB;
    float gyroDPSDigit;

    // Enumerations
    typedef enum
    {
        WHO_AM_I = 0x0F
    } CommonRegisters;

    typedef enum
    {
        CTRL_REG1_G     = 0x10,
        CTRL_REG2_G     = 0x11,
        CTRL_REG3_G     = 0x12,

        TEMP_OUT_L      = 0x15,
        TEMP_OUT_H      = 0x16,

        STATUS_REG      = 0x17,

        GYRO_OUT_X_L    = 0x18,
        GYRO_OUT_X_H    = 0x19,
        GYRO_OUT_Y_L    = 0x1A,
        GYRO_OUT_Y_H    = 0x1B,
        GYRO_OUT_Z_L    = 0x1C,
        GYRO_OUT_Z_H    = 0x1D,

        CTRL_REG4       = 0x1E,
        CTRL_REG5_XL    = 0x1F,
        CTRL_REG6_XL    = 0x20,
        CTRL_REG7_XL    = 0x21,

        XL_OUT_X_L      = 0x28,
        XL_OUT_X_H      = 0x29,
        XL_OUT_Y_L      = 0x2A,
        XL_OUT_Y_H      = 0x2B,
        XL_OUT_Z_L      = 0x2C,
        XL_OUT_Z_H      = 0x2D

    } XGRegisters;

    typedef enum
    {
        CTRL_REG1_M     = 0x20,
        CTRL_REG2_M     = 0x21,
        CTRL_REG3_M     = 0x22,
        CTRL_REG4_M     = 0x23,
        CTRL_REG5_M     = 0x24,

        STATUS_REG_M    = 0x27,

        M_OUT_X_L       = 0x28,
        M_OUT_X_H       = 0x29,
        M_OUT_Y_L       = 0x2A,
        M_OUT_Y_H       = 0x2B,
        M_OUT_Z_L       = 0x2C,
        M_OUT_Z_H       = 0x2D,

        CFG_M           = 0x30,
        INT_SRC_M       = 0x31

    } MagRegisters;

    typedef enum
    {
        XL_2G  = (0b00 << 3),
        XL_16G = (0b01 << 3),
        XL_4G  = (0b10 << 3),
        XL_8G  = (0b11 << 3)

    } AccelRange;

    // DPS = Degrees per second rotation
    typedef enum
    {
        GYRO_SCALE_245DPS   = (0b00 << 4),
        GYRO_SCALE_500DPS   = (0b01 << 4),
        GYRO_SCALE_2000DPS  = (0b11 << 4)
    } GyroScale;

    typedef enum
    {
        MAG_4GAUSS  = (0b00 << 5),
        MAG_8GAUSS  = (0b01 << 5),
        MAG_12GAUSS = (0b10 << 5),
        MAG_16GAUSS = (0b11 << 5)
    } MagGain;

    typedef enum
    {
        XDR_POWER_DOWN  = (0b0000 << 4),
        XDR_3_125HZ     = (0b0001 << 4),
        XDR_6_25HZ      = (0b0010 << 4),
        XDR_12_5HZ      = (0b0011 << 4),
        XDR_25HZ        = (0b0100 << 4),
        XDR_50HZ        = (0b0101 << 4),
        XDR_100HZ       = (0b0110 << 4),
        XDR_200HZ       = (0b0111 << 4),
        XDR_400HZ       = (0b1000 << 4),
        XDR_800HZ       = (0b1001 << 4),
        XDR_1600HZ      = (0b1010 << 4)
    } XDataRate;

    typedef enum
    {
        MDR_3_125HZ     = (0b000 << 2),
        MDR_6_25HZ      = (0b001 << 2),
        MDR_12_5HZ      = (0b010 << 2),
        MDR_25HZ        = (0b011 << 2),
        MDR_50HZ        = (0b100 << 2),
        MDR_100HZ       = (0b101 << 2)
    } MagDataRate;

    // Functions
    void readBuffer(bool type, uint8_t *buf);

    void setupMag(MagGain gain);
    void setupAccel(AccelRange range);
    void setupGyro(GyroScale scale);

};

#endif // LSM9DS1_H
