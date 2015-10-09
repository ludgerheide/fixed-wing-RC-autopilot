//
//  bmp.h
//  avr - flyByWire
//
//  Created by Ludger Heide on 27.09.15.
//  Copyright © 2015 LH Technologies. All rights reserved.
//

#ifndef bmp_h
#define bmp_h

#include <stdio.h>
#include "avrlibtypes.h"

/*=========================================================================
 I2C ADDRESS/BITS
 -----------------------------------------------------------------------*/
#define BMP180_ADDRESS                (0x77)
/*=========================================================================*/

#define BMP180_TEMPERATURE_DURATION 5
#define BMP180_PRESSURE_DURATION 26

/*=========================================================================
 REGISTERS
 -----------------------------------------------------------------------*/
enum
{
    BMP180_REGISTER_CAL_AC1            = 0xAA,  // R   Calibration data (16 bits)
    BMP180_REGISTER_CAL_AC2            = 0xAC,  // R   Calibration data (16 bits)
    BMP180_REGISTER_CAL_AC3            = 0xAE,  // R   Calibration data (16 bits)
    BMP180_REGISTER_CAL_AC4            = 0xB0,  // R   Calibration data (16 bits)
    BMP180_REGISTER_CAL_AC5            = 0xB2,  // R   Calibration data (16 bits)
    BMP180_REGISTER_CAL_AC6            = 0xB4,  // R   Calibration data (16 bits)
    BMP180_REGISTER_CAL_B1             = 0xB6,  // R   Calibration data (16 bits)
    BMP180_REGISTER_CAL_B2             = 0xB8,  // R   Calibration data (16 bits)
    BMP180_REGISTER_CAL_MB             = 0xBA,  // R   Calibration data (16 bits)
    BMP180_REGISTER_CAL_MC             = 0xBC,  // R   Calibration data (16 bits)
    BMP180_REGISTER_CAL_MD             = 0xBE,  // R   Calibration data (16 bits)
    BMP180_REGISTER_CHIPID             = 0xD0,
    BMP180_REGISTER_VERSION            = 0xD1,
    BMP180_REGISTER_SOFTRESET          = 0xE0,
    BMP180_REGISTER_CONTROL            = 0xF4,
    BMP180_REGISTER_TEMPDATA           = 0xF6,
    BMP180_REGISTER_PRESSUREDATA       = 0xF6,
    BMP180_REGISTER_READTEMPCMD        = 0x2E,
    BMP180_REGISTER_READPRESSURECMD    = 0x34,
    BMP180_REGISTER_READPRESSURECMD_UHR= 0xF4   //Ultra High-res
};
/*=========================================================================*/

//Structs
/*=========================================================================
 MODE SETTINGS
 -----------------------------------------------------------------------*/
typedef enum
{
    BMP180_MODE_ULTRALOWPOWER          = 0,
    BMP180_MODE_STANDARD               = 1,
    BMP180_MODE_HIGHRES                = 2,
    BMP180_MODE_ULTRAHIGHRES           = 3
} BMP180_mode_t;
/*=========================================================================*/

typedef struct {
    float temperature;
    float pressure;
    u32 timestamp;
} pressureEvent;

typedef struct tempPressRawData {
    u08 templo;
    u08 temphi;
    u08 presshi;
    u08 presslo;
    u08 pressxlo;
    u32 timestamp;
} tempPressRawData;

typedef enum {
    IDLE,
    TEMPERATURE_MEASURING,
    TEMPERATURE_READY,
    TEMPERATURE_RECEIVING,
    PRESSURE_MEASURING,
    PRESSURE_READY,
    PRESSURE_RECEIVING
} bmpState;

//Variables
extern bmpState myBmpState;
extern tempPressRawData myBmpRawData;
extern u32 bmpLastStateChange;

//Initialize the gyro
//Returns true if the initializationw as successfull, false otherwise
BOOL bmpInit(void);

//Start the temperature capture
void bmpStartTemperatureCapture(void);

//Send the "get temperature" request
void bmpStartReceivingTemperature(void);

//Start the pressure capture
void bmpStartPressureCapture(void);

//Send the "get pressure" request
void bmpStartReceivingPressure(void);

//Copy the data out of the receiving buffer into our struct
void bmpGetPressDataFromI2cBuffer(void);

//Copy the data out of the receiving buffer into our struct
void bmpGetTempDataFromI2cBuffer(void);

//Gets the altitude from the bmp
void bmpGetData(pressureEvent* myEvent);

//Convert a pressure reading to altitude
float pressureToAltitude(float atmosphericPressure, float theSeaLevelPressure);

#endif /* bmp_h */