//
//  gyro.c
//  avr - flyByWire
//
//  Created by Ludger Heide on 24.09.15.
//  Copyright © 2015 LH Technologies. All rights reserved.
//

#include "gyro.h"
#include "i2c.h"

#include <assert.h>
#include <avr/io.h>

#ifndef CRITICAL_SECTION_START
#define CRITICAL_SECTION_START	unsigned char _sreg = SREG; cli()
#define CRITICAL_SECTION_END	SREG = _sreg
#endif

//Global variables
gyroRawData myGyroRawData;
gyroRange_t selectedRange;

/*=========================================================================
 I2C ADDRESS/BITS AND SETTINGS
 -----------------------------------------------------------------------*/
#define L3GD20_ID                (0xD4)
#define GYRO_SENSITIVITY_250DPS  (0.00875F)    // Roughly 22/256 for fixed point match
#define GYRO_SENSITIVITY_500DPS  (0.0175F)     // Roughly 45/256
#define GYRO_SENSITIVITY_2000DPS (0.070F)      // Roughly 18/256
#define SENSORS_DPS_TO_RADS      (0.017453293F)/**< Degrees/s to rad/s multiplier */
/*=========================================================================*/

//Write a byte to a specified register
static void gyroWrite8(u08 reg, u08 value) {
    
    //Combine reg and value in a 16 byt variable
    u08 transmission[2] = {reg, value};
    
    u08 i2cstat = i2cMasterSendNI(L3GD20_ADDRESS, 2, (u08*)&transmission);
    assert(i2cstat == I2C_OK);
}

//Read from a specified register
static u08 gyroRead8(u08 reg) {
    u08 i2cstat = i2cMasterSendNI(L3GD20_ADDRESS, 1, &reg);
    assert(i2cstat == I2C_OK);
    
    u08 outByte;
    i2cstat = i2cMasterReceiveNI(L3GD20_ADDRESS, 1, &outByte);
    assert(i2cstat == I2C_OK);
    
    return outByte;
}

//Initialize the gyro
//Returns true if the initializationw as successfull, false otherwise
BOOL gyroInit(gyroRange_t range) {
    
    //Check if we have the correct chip
    u08 id = gyroRead8(GYRO_REGISTER_WHO_AM_I);
    
    if(id != L3GD20_ID) {
        return FALSE;
    }
    
    /* Set CTRL_REG1 (0x20)
     ====================================================================
     BIT  Symbol    Description                                   Default
     ---  ------    --------------------------------------------- -------
     7-6  DR1/0     Output data rate                                   00
     5-4  BW1/0     Bandwidth selection                                00
     3  PD        0 = Power-down mode, 1 = normal/sleep mode          0
     2  ZEN       Z-axis enable (0 = disabled, 1 = enabled)           1
     1  YEN       Y-axis enable (0 = disabled, 1 = enabled)           1
     0  XEN       X-axis enable (0 = disabled, 1 = enabled)           1 */
    
    /* Reset then switch to normal mode and enable all three channels */
    gyroWrite8(GYRO_REGISTER_CTRL_REG1, 0x00);
    gyroWrite8(GYRO_REGISTER_CTRL_REG1, 0x0F);
    /* ------------------------------------------------------------------ */
    
    /* Set CTRL_REG2 (0x21)
     ====================================================================
     BIT  Symbol    Description                                   Default
     ---  ------    --------------------------------------------- -------
     5-4  HPM1/0    High-pass filter mode selection                    00
     3-0  HPCF3..0  High-pass filter cutoff frequency selection      0000 */
    
    /* Nothing to do ... keep default values */
    /* ------------------------------------------------------------------ */
    
    /* Set CTRL_REG3 (0x22)
     ====================================================================
     BIT  Symbol    Description                                   Default
     ---  ------    --------------------------------------------- -------
     7  I1_Int1   Interrupt enable on INT1 (0=disable,1=enable)       0
     6  I1_Boot   Boot status on INT1 (0=disable,1=enable)            0
     5  H-Lactive Interrupt active config on INT1 (0=high,1=low)      0
     4  PP_OD     Push-Pull/Open-Drain (0=PP, 1=OD)                   0
     3  I2_DRDY   Data ready on DRDY/INT2 (0=disable,1=enable)        0
     2  I2_WTM    FIFO wtrmrk int on DRDY/INT2 (0=dsbl,1=enbl)        0
     1  I2_ORun   FIFO overrun int on DRDY/INT2 (0=dsbl,1=enbl)       0
     0  I2_Empty  FIFI empty int on DRDY/INT2 (0=dsbl,1=enbl)         0 */
    gyroWrite8(GYRO_REGISTER_CTRL_REG3, 0x00);
    
    /* ------------------------------------------------------------------ */
    
    /* Set CTRL_REG4 (0x23)
     ====================================================================
     BIT  Symbol    Description                                   Default
     ---  ------    --------------------------------------------- -------
     7  BDU       Block Data Update (0=continuous, 1=LSB/MSB)         0
     6  BLE       Big/Little-Endian (0=Data LSB, 1=Data MSB)          0
     5-4  FS1/0     Full scale selection                               00
     00 = 250 dps
     01 = 500 dps
     10 = 2000 dps
     11 = 2000 dps
     0  SIM       SPI Mode (0=4-wire, 1=3-wire)                       0 */
    
    /* Adjust resolution if requested */
    selectedRange = range;
    switch(range) {
        case GYRO_RANGE_250DPS:
            gyroWrite8(GYRO_REGISTER_CTRL_REG4, 0x00);
            break;
        case GYRO_RANGE_500DPS:
            gyroWrite8(GYRO_REGISTER_CTRL_REG4, 0x10);
            break;
        case GYRO_RANGE_2000DPS:
            gyroWrite8(GYRO_REGISTER_CTRL_REG4, 0x20);
            break;
    }
    /* ------------------------------------------------------------------ */
    
    /* Set CTRL_REG5 (0x24)
     ====================================================================
     BIT  Symbol    Description                                   Default
     ---  ------    --------------------------------------------- -------
     7  BOOT      Reboot memory content (0=normal, 1=reboot)          0
     6  FIFO_EN   FIFO enable (0=FIFO disable, 1=enable)              0
     4  HPen      High-pass filter enable (0=disable,1=enable)        0
     3-2  INT1_SEL  INT1 Selection config                              00
     1-0  OUT_SEL   Out selection config                               00 */
    
    /* Nothing to do ... keep default values */
    /* ------------------------------------------------------------------ */
    
    return TRUE;
}

void gyroDisableDrdy(void) {
    gyroWrite8(GYRO_REGISTER_CTRL_REG3, 0x00);
}

void gyroEnableDrdy(void) {
    gyroWrite8(GYRO_REGISTER_CTRL_REG3, 0x08);
}

//Send the "get data" request
void gyroSendDataRequest(void) {
    u08 gyroDataRequest = GYRO_REGISTER_OUT_X_L | 0x80;
    i2cMasterSend(L3GD20_ADDRESS, 1, &gyroDataRequest);
}

//Gets the data from the gyro
void gyroGetData(gyroEvent* myEvent) {
    CRITICAL_SECTION_START;
    /* Shift values to create properly formed integer (low byte first) */
    s16 rawX = (s16)(myGyroRawData.xlo | (myGyroRawData.xhi << 8));
    s16 rawY = (s16)(myGyroRawData.ylo | (myGyroRawData.yhi << 8));
    s16 rawZ = (s16)(myGyroRawData.zlo | (myGyroRawData.zhi << 8));
    
    //Set the timestamp
    myEvent->timestamp = myGyroRawData.timestamp;
    CRITICAL_SECTION_END;
    switch (selectedRange) {
        case GYRO_RANGE_250DPS:
            myEvent->x = (GYRO_SENSITIVITY_250DPS * SENSORS_DPS_TO_RADS) * rawX;
            myEvent->y = (GYRO_SENSITIVITY_250DPS * SENSORS_DPS_TO_RADS) * rawY;
            myEvent->z = (GYRO_SENSITIVITY_250DPS * SENSORS_DPS_TO_RADS) * rawZ;
            break;
            
        case GYRO_RANGE_500DPS:
            myEvent->x = (GYRO_SENSITIVITY_500DPS * SENSORS_DPS_TO_RADS) * rawX;
            myEvent->y = (GYRO_SENSITIVITY_500DPS * SENSORS_DPS_TO_RADS) * rawY;
            myEvent->z = (GYRO_SENSITIVITY_500DPS * SENSORS_DPS_TO_RADS) * rawZ;
            break;
            
        case GYRO_RANGE_2000DPS:
            myEvent->x = (GYRO_SENSITIVITY_2000DPS * SENSORS_DPS_TO_RADS) * rawX;
            myEvent->y = (GYRO_SENSITIVITY_2000DPS * SENSORS_DPS_TO_RADS) * rawY;
            myEvent->z = (GYRO_SENSITIVITY_2000DPS * SENSORS_DPS_TO_RADS) * rawZ;
            break;
            
        default:
            break;
    }
}