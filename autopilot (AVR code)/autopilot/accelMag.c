//
//  accelMag.c
//  avr - flyByWire
//
//  Created by Ludger Heide on 27.09.15.
//  Copyright © 2015 LH Technologies. All rights reserved.
//

#include "accelMag.h"
#include "i2c.h"
#include <assert.h>
#include <avr/io.h>

#ifndef _unused
#define _unused(x) ((void)x)
#endif

#ifndef CRITICAL_SECTION_START
#define CRITICAL_SECTION_START	unsigned char _sreg = SREG; cli()
#define CRITICAL_SECTION_END	SREG = _sreg
#endif

/*=========================================================================
 MAGNETOMETER GAIN SETTINGS
 -----------------------------------------------------------------------*/
typedef enum
{
    LSM303_MAGGAIN_1_3                        = 0x20,  // +/- 1.3
    LSM303_MAGGAIN_1_9                        = 0x40,  // +/- 1.9
    LSM303_MAGGAIN_2_5                        = 0x60,  // +/- 2.5
    LSM303_MAGGAIN_4_0                        = 0x80,  // +/- 4.0
    LSM303_MAGGAIN_4_7                        = 0xA0,  // +/- 4.7
    LSM303_MAGGAIN_5_6                        = 0xC0,  // +/- 5.6
    LSM303_MAGGAIN_8_1                        = 0xE0   // +/- 8.1
} lsm303MagGain;
/*=========================================================================*/

//Variables
accelRawData myAccelRawData;
magRawData myMagRawData;

//Methods
//Write a byte to a specified register
static void accelWrite8(u08 reg, u08 value) {
    
    //Combine reg and value in a 16 byt variable
    u08 transmission[2] = {reg, value};
    
    u08 i2cstat = i2cMasterSendNI(LSM303_ADDRESS_ACCEL, 2, (u08*)&transmission);
    assert(i2cstat == I2C_OK);
    _unused(i2cstat);
}

//Read from a specified register
static u08 accelRead8(u08 reg) {
    u08 i2cstat = i2cMasterSendNI(LSM303_ADDRESS_ACCEL, 1, &reg);
    assert(i2cstat == I2C_OK);
    
    u08 outByte;
    i2cstat = i2cMasterReceiveNI(LSM303_ADDRESS_ACCEL, 1, &outByte);
    assert(i2cstat == I2C_OK);
    _unused(i2cstat);
    
    return outByte;
}

//Write a byte to a specified register
static void magWrite8(u08 reg, u08 value) {
    
    //Combine reg and value in a 16 byt variable
    u08 transmission[2] = {reg, value};
    
    u08 i2cstat = i2cMasterSendNI(LSM303_ADDRESS_MAG, 2, (u08*)&transmission);
    assert(i2cstat == I2C_OK);
    _unused(i2cstat);
}

//Read from a specified register
static u08 magRead8(u08 reg) {
    u08 i2cstat = i2cMasterSendNI(LSM303_ADDRESS_MAG, 1, &reg);
    assert(i2cstat == I2C_OK);
    _unused(i2cstat);
    
    u08 outByte;
    i2cstat = i2cMasterReceiveNI(LSM303_ADDRESS_MAG, 1, &outByte);
    assert(i2cstat == I2C_OK);
    _unused(i2cstat);
    
    return outByte;
}



BOOL accelInit(void) {
    const u08 regSetting = 0x67; //0x67 for 200 Hz, 0x57 for 100Hz, 0x27 for 10Hz
    
    // Enable the accelerometer
    accelWrite8(LSM303_REGISTER_ACCEL_CTRL_REG1_A, regSetting);
    
    //Disable the LIN1 DRDY interrupt
    accelWrite8(LSM303_REGISTER_ACCEL_CTRL_REG3_A, 0x00);
    
    // LSM303DLHC has no WHOAMI register so read CTRL_REG1_A back to check
    // if we are connected or not
    u08 reg1_a = accelRead8(LSM303_REGISTER_ACCEL_CTRL_REG1_A);
    if(reg1_a != regSetting) {
        return FALSE;
    }
    return TRUE;
}

BOOL magInit(void) {
    // Enable the magnetometer
    magWrite8(LSM303_REGISTER_MAG_MR_REG_M, 0x00);
    
    //Set the update rate to 75Hz
    const u08 regSetting = 0x1C; //0x1C for 220Hz, 0x18 for 75Hz, 0x08 for 3Hz
    magWrite8(LSM303_REGISTER_MAG_CRA_REG_M, regSetting);
    
    u08 reg_cra_m = magRead8(LSM303_REGISTER_MAG_CRA_REG_M);
    if (reg_cra_m != regSetting) {
        return FALSE;
    }
    
    //Set the gain
    magWrite8(LSM303_REGISTER_MAG_CRB_REG_M, LSM303_MAGGAIN_1_3);
    return TRUE;
}

void accelDisableDrdy(void) {
    //Disable the LIN1 DRDY interrupt
    accelWrite8(LSM303_REGISTER_ACCEL_CTRL_REG3_A, 0x00);
}

void accelEnableDrdy(void) {
    //Enable the LIN1 DRDY interrupt
    accelWrite8(LSM303_REGISTER_ACCEL_CTRL_REG3_A, 0x10);
}

void accelSendDataRequest(void) {
    u08 accelDataRequest = (LSM303_REGISTER_ACCEL_OUT_X_L_A | 0x80);
    i2cMasterSend(LSM303_ADDRESS_ACCEL, 1, &accelDataRequest);
}

void magSendDataRequest(void) {
    u08 magDataRequest = LSM303_REGISTER_MAG_OUT_X_H_M;
    i2cMasterSend(LSM303_ADDRESS_MAG, 1, &magDataRequest);
}

void accelGetData(accelEvent* myEvent) {
    CRITICAL_SECTION_START;
    /* Shift values to create properly formed integer (low byte first) */
    s16 rawX = (s16)(myAccelRawData.xlo | (myAccelRawData.xhi << 8)) >> 4;
    s16 rawY = (s16)(myAccelRawData.ylo | (myAccelRawData.yhi << 8)) >> 4;
    s16 rawZ = (s16)(myAccelRawData.zlo | (myAccelRawData.zhi << 8)) >> 4;
    
    //Set the timestamp
    myEvent->timestamp = myAccelRawData.timestamp;
    CRITICAL_SECTION_END;
    
    const float lsm303Accel_MG_LSB     = 0.001F;   // 1, 2, 4 or 12 mg per lsb
    const float earthGravity           = 9.80665F;

    myEvent->x = rawX * lsm303Accel_MG_LSB * earthGravity;
    myEvent->y = rawY * lsm303Accel_MG_LSB * earthGravity;
    myEvent->z = rawZ * lsm303Accel_MG_LSB * earthGravity;
}

void magGetData(magEvent* myEvent) {
    CRITICAL_SECTION_START;
    /* Shift values to create properly formed integer (low byte first) */
    s16 rawX = (s16)(myMagRawData.xlo | (myMagRawData.xhi << 8));
    s16 rawY = (s16)(myMagRawData.ylo | (myMagRawData.yhi << 8));
    s16 rawZ = (s16)(myMagRawData.zlo | (myMagRawData.zhi << 8));
    
    //Set the timestamp
    myEvent->timestamp = myMagRawData.timestamp;
    CRITICAL_SECTION_END;
    
    static float lsm303Mag_Gauss_LSB_XY = 1100.0F;  // Varies with gain
    static float lsm303Mag_Gauss_LSB_Z  = 980.0F;   // Varies with gain
    static u08   gauss_to_microtesla    = 100;
    
    myEvent->x = rawX / lsm303Mag_Gauss_LSB_XY * gauss_to_microtesla;
    myEvent->y = rawY / lsm303Mag_Gauss_LSB_XY * gauss_to_microtesla;
    myEvent->z = rawZ / lsm303Mag_Gauss_LSB_Z * gauss_to_microtesla;
}

//Compensates a magnetometer using a matrix calculated using ellipsoid fitting on the raw data
//The calibration code (matlab) is oncluded
// x    m11 m12 m13    xin - offsetx
// y =  m21 m22 m23 * (yin - offsety)
// z    m31 m32 m33    zin - offsetz
void magCompensate(magEvent* input, magEvent* output) {
const float magn_ellipsoid_center[3] = {1.03434, 3.30038, -5.08871};
const float magn_ellipsoid_transform[3][3] = {{0.977877, -0.00435189, 0.0135533}, {-0.00435189, 0.908601, -0.019884}, {0.0135533, -0.019884, 0.986081}};

    //Forst, calibrate the offset
    output->x = input->x - magn_ellipsoid_center[0];
    output->y = input->y - magn_ellipsoid_center[1];
    output->z = input->z - magn_ellipsoid_center[2];
    
    //Then, do the matrix-vextor multiplication
    output->x = output->x * magn_ellipsoid_transform[0][0] + output->y * magn_ellipsoid_transform[0][1] + output->z * magn_ellipsoid_transform[0][2];
    output->y = output->x * magn_ellipsoid_transform[1][0] + output->y * magn_ellipsoid_transform[1][1] + output->z * magn_ellipsoid_transform[1][2];
    output->z = output->x * magn_ellipsoid_transform[2][0] + output->y * magn_ellipsoid_transform[2][1] + output->z * magn_ellipsoid_transform[2][2];
}