//
//  utils.c
//  avr - flyByWire
//
//  Created by Ludger Heide on 23.09.15.
//  Copyright © 2015 LH Technologies. All rights reserved.
//

#include "utils.h"
#include "uart4.h"
#include <assert.h>
#include <avr/eeprom.h>
#include <math.h>
#include "pinSetup.h"

#ifndef CRITICAL_SECTION_START
#define CRITICAL_SECTION_START	unsigned char _sreg = SREG; cli()
#define CRITICAL_SECTION_END	SREG = _sreg
#endif

//EEPROM layout
#define EEPROM_SLP_ADDRESS 0x00 //Stores a float, next address 0x04
#define EEPROM_HOMEBASE_ADDRESS 0x04

//Default home base if the one we have is corrupoted or zero
static const waypoint defaultHomeBase = {.timestamp = 1,
                                         .latitude = 13.1696819,
                                         .longitude = 52.5020173,
                                         .altitude = 80 };

static FILE mystdout = FDEV_SETUP_STREAM(raspiPutChar, NULL, _FDEV_SETUP_WRITE);

//Static function definitions
static float toRadians(float degrees);
static float toDegrees(float radians);

void printfAttachToUart(void) {
    //Initialize UART 3 and make it stderr and stdout
    uartInit(RASPI_UART);
    uartSetBaudRate(RASPI_UART, 115200);
    stderr = &mystdout;
    stdout = &mystdout;
}

//Standards-compliant putchar function that sends stuff to the raspberry pi
int raspiPutChar(char c, __attribute__ ((unused)) FILE* stream) {
    uartSendByte(RASPI_UART, c);
    return 0;
}

//Utility function that puts a string into a UART byte-wise
void uartPutStr(u08 nUart, char* string) {
    assert(nUart < 4);
    
    while(*string != '\0') {
        uartSendByte(nUart, *string);
        string++;
    }
}

//Reads in the sea level pressure from eeprom and checks its validity
void readSlpFromEEPROM(void) {
    eeprom_busy_wait();
    CRITICAL_SECTION_START;
    float slpFromRom = eeprom_read_float(EEPROM_SLP_ADDRESS);
    CRITICAL_SECTION_END;
    
    if(slpFromRom <= 1100 && slpFromRom >= 850) {
        //Our value is within the allowed bounds
        seaLevelPressure = slpFromRom;
    } else {
        seaLevelPressure = 1013; //Stanard pressure otherwise
    }
}

void writeSlpToEEPROM(void) {
    eeprom_busy_wait();
    CRITICAL_SECTION_START;
    eeprom_update_float(EEPROM_SLP_ADDRESS, seaLevelPressure);
    CRITICAL_SECTION_END;
}

//Reads in the home base from eeprom and checks its validity
void readHomeBaseFromEEPROM(void) {
    //Obivously invalid defaults
    waypoint tmpHomeBase = {.timestamp = 1,
                            .latitude = 91,
                            .longitude = 181,
                            .altitude = -500 };
    
    //Load the home base from EEPROM
    eeprom_busy_wait();
    CRITICAL_SECTION_START;
    eeprom_read_block(&tmpHomeBase, (void*)EEPROM_HOMEBASE_ADDRESS, sizeof(waypoint));
    CRITICAL_SECTION_END;
    
    //Verify the coordinates
    if(isnan(tmpHomeBase.latitude) || tmpHomeBase.latitude > 90 || tmpHomeBase.latitude < -90 || isnan(tmpHomeBase.longitude) || tmpHomeBase.longitude > 180 || tmpHomeBase.longitude < -180 || isnan(tmpHomeBase.altitude) || tmpHomeBase.altitude < -418 || tmpHomeBase.altitude > 10000) {
        //Note: -418 is the lowest surface point in the world
        homeBase = defaultHomeBase;
    } else {
        tmpHomeBase.timestamp = 1;
        homeBase = tmpHomeBase;
    }
}

//Writes the home base to eeprom
void writeHomeBaseToEEPROM(void) {
    //Write the homebase to the eeprom
    eeprom_busy_wait();
    CRITICAL_SECTION_START;
    eeprom_update_block(&homeBase, (void*)EEPROM_HOMEBASE_ADDRESS, sizeof(waypoint));
    CRITICAL_SECTION_END;
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
    float out = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    if(out < out_min) {
        out = out_min;
    } else if(out > out_max) {
        out = out_max;
    }
    return out;
}

s32 maps32(s32 x, s32 in_min, s32 in_max, s32 out_min, s32 out_max) {
    s32 out = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    if(out < out_min) {
        out = out_min;
    } else if(out > out_max) {
        out = out_max;
    }
    return out;
}

s16 maps16(s16 x, s16 in_min, s16 in_max, s16 out_min, s16 out_max) {
    s16 out = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    if(out < out_min) {
        out = out_min;
    } else if(out > out_max) {
        out = out_max;
    }
    return out;
}

u16 bearingToCoordinates(float fromPhi, float fromLambda, float toPhi, float toLambda) {
    assert(fromPhi <= 90 && fromPhi >= -90 && fromLambda <= 180 && fromLambda >= -180);
    assert(toPhi <= 90 && toPhi >= -90 && toLambda <= 180 && toLambda >= -180);
    
    float phi1 = toRadians(fromPhi);
    float phi2 = toRadians(toPhi);
    
    float lambda1 = toRadians(fromLambda);
    float lambda2 = toRadians(toLambda);
    
    float y = sin(lambda2 - lambda1) * cos(phi2);
    float x = cos(phi1) * sin(phi2) - sin(phi1) * cos(phi2) * cos(lambda2 - lambda1);
    
    s16 bearing = (s16)roundf(toDegrees(atan2(y,x))) % 360;
    if(bearing < 0) {
        bearing += 360;
    }
    
    return (u16)bearing;
}

static float toRadians(float degrees) {
    return degrees / 180.0 * M_PI;
}

static float toDegrees(float radians) {
    return radians * 180.0 / M_PI;
}