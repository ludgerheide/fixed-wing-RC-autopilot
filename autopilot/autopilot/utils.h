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

#endif /* utils_h */