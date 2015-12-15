//
//  utils.h
//  avr - flyByWire
//
//  Created by Ludger Heide on 23.09.15.
//  Copyright © 2015 LH Technologies. All rights reserved.
//

#ifndef utils_h
#define utils_h

#include <stdio.h>
#include "avrlibtypes.h"

//Attaches the prinbtf function to the raspi UART
void printfAttachToUart(void);

//Putchar function to work with the avrlib uart and avr-libc stdio (sends stdout, stdin, stderr to RasPi
int raspiPutChar(char c, FILE* stream);

//Utility function that puts a string into a UART byte-wise
void uartPutStr(u08 nUart, char* string);

//Reads in the sea level pressure from eeprom and checks its validity
void readSlpFromEEPROM(void);

//Maps a float from one range to another, making sure it's in bounds
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);

//Same for s32
s32 maps32(s32 x, s32 in_min, s32 in_max, s32 out_min, s32 out_max);

//Same for s16
s16 maps16(s16 x, s16 in_min, s16 in_max, s16 out_min, s16 out_max);

#endif /* utils_h */