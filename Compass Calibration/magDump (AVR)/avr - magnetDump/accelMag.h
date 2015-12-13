//
//  accelMag.h
//  avr - flyByWire
//
//  Created by Ludger Heide on 27.09.15.
//  Copyright © 2015 LH Technologies. All rights reserved.
//

#ifndef accelMag_h
#define accelMag_h

#include <stdio.h>
#include "avrlibtypes.h"

/*=========================================================================
 I2C ADDRESS/BITS
 -----------------------------------------------------------------------*/
#define LSM303_ADDRESS_ACCEL          (0x32 >> 1)         // 0011001x
#define LSM303_ADDRESS_MAG            (0x3C >> 1)         // 0011110x
/*=========================================================================*/

/*=========================================================================
 REGISTERS
 -----------------------------------------------------------------------*/
typedef enum
{                                                     // DEFAULT    TYPE
    LSM303_REGISTER_ACCEL_CTRL_REG1_A         = 0x20,   // 00000111   rw
    LSM303_REGISTER_ACCEL_CTRL_REG2_A         = 0x21,   // 00000000   rw
    LSM303_REGISTER_ACCEL_CTRL_REG3_A         = 0x22,   // 00000000   rw
    LSM303_REGISTER_ACCEL_CTRL_REG4_A         = 0x23,   // 00000000   rw
    LSM303_REGISTER_ACCEL_CTRL_REG5_A         = 0x24,   // 00000000   rw
    LSM303_REGISTER_ACCEL_CTRL_REG6_A         = 0x25,   // 00000000   rw
    LSM303_REGISTER_ACCEL_REFERENCE_A         = 0x26,   // 00000000   r
    LSM303_REGISTER_ACCEL_STATUS_REG_A        = 0x27,   // 00000000   r
    LSM303_REGISTER_ACCEL_OUT_X_L_A           = 0x28,
    LSM303_REGISTER_ACCEL_OUT_X_H_A           = 0x29,
    LSM303_REGISTER_ACCEL_OUT_Y_L_A           = 0x2A,
    LSM303_REGISTER_ACCEL_OUT_Y_H_A           = 0x2B,
    LSM303_REGISTER_ACCEL_OUT_Z_L_A           = 0x2C,
    LSM303_REGISTER_ACCEL_OUT_Z_H_A           = 0x2D,
    LSM303_REGISTER_ACCEL_FIFO_CTRL_REG_A     = 0x2E,
    LSM303_REGISTER_ACCEL_FIFO_SRC_REG_A      = 0x2F,
    LSM303_REGISTER_ACCEL_INT1_CFG_A          = 0x30,
    LSM303_REGISTER_ACCEL_INT1_SOURCE_A       = 0x31,
    LSM303_REGISTER_ACCEL_INT1_THS_A          = 0x32,
    LSM303_REGISTER_ACCEL_INT1_DURATION_A     = 0x33,
    LSM303_REGISTER_ACCEL_INT2_CFG_A          = 0x34,
    LSM303_REGISTER_ACCEL_INT2_SOURCE_A       = 0x35,
    LSM303_REGISTER_ACCEL_INT2_THS_A          = 0x36,
    LSM303_REGISTER_ACCEL_INT2_DURATION_A     = 0x37,
    LSM303_REGISTER_ACCEL_CLICK_CFG_A         = 0x38,
    LSM303_REGISTER_ACCEL_CLICK_SRC_A         = 0x39,
    LSM303_REGISTER_ACCEL_CLICK_THS_A         = 0x3A,
    LSM303_REGISTER_ACCEL_TIME_LIMIT_A        = 0x3B,
    LSM303_REGISTER_ACCEL_TIME_LATENCY_A      = 0x3C,
    LSM303_REGISTER_ACCEL_TIME_WINDOW_A       = 0x3D
} lsm303AccelRegisters_t;

typedef enum
{
    LSM303_REGISTER_MAG_CRA_REG_M             = 0x00,
    LSM303_REGISTER_MAG_CRB_REG_M             = 0x01,
    LSM303_REGISTER_MAG_MR_REG_M              = 0x02,
    LSM303_REGISTER_MAG_OUT_X_H_M             = 0x03,
    LSM303_REGISTER_MAG_OUT_X_L_M             = 0x04,
    LSM303_REGISTER_MAG_OUT_Z_H_M             = 0x05,
    LSM303_REGISTER_MAG_OUT_Z_L_M             = 0x06,
    LSM303_REGISTER_MAG_OUT_Y_H_M             = 0x07,
    LSM303_REGISTER_MAG_OUT_Y_L_M             = 0x08,
    LSM303_REGISTER_MAG_SR_REG_Mg             = 0x09,
    LSM303_REGISTER_MAG_IRA_REG_M             = 0x0A,
    LSM303_REGISTER_MAG_IRB_REG_M             = 0x0B,
    LSM303_REGISTER_MAG_IRC_REG_M             = 0x0C,
    LSM303_REGISTER_MAG_TEMP_OUT_H_M          = 0x31,
    LSM303_REGISTER_MAG_TEMP_OUT_L_M          = 0x32
} lsm303MagRegisters_t;
/*=========================================================================*/


//Structs
typedef struct {
    float x;
    float y;
    float z;
    u32 timestamp;
} accelEvent;

typedef struct {
    float x;
    float y;
    float z;
    u32 timestamp;
} magEvent;

typedef struct accelRawData {
    u08 xlo;
    u08 xhi;
    u08 ylo;
    u08 yhi;
    u08 zlo;
    u08 zhi;
    u32 timestamp;
} accelRawData;

typedef struct magRawData {
    u08 xlo;
    u08 xhi;
    u08 ylo;
    u08 yhi;
    u08 zlo;
    u08 zhi;
    u32 timestamp;
} magRawData;

//Variables
extern accelRawData myAccelRawData;
extern magRawData myMagRawData;

//Methods
BOOL accelInit(void);
BOOL magInit(void);

void accelDisableDrdy(void);
void accelEnableDrdy(void);

void accelSendDataRequest(void);
void magSendDataRequest(void);

void accelGetData(accelEvent* myEvent);
void magGetData(magEvent* myEvent);

//Compensates a magnetometer using a matrix calculated using ellipsoid fitting on the raw data
//The calibration code (matlab) is oncluded
// x    m11 m12 m13    xin - offsetx
// y =  m21 m22 m23 * (yin - offsety)
// z    m31 m32 m33    zin - offsetz
void magCompensate(magEvent* input, magEvent* output);

#endif /* accelMag_h */