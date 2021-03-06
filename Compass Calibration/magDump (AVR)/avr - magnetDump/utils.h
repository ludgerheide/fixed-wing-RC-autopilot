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

//Attaches the prinbtf function to the raspi UART
void printfAttachToUart(void);

//Putchar function to work with the avrlib uart and avr-libc stdio (sends stdout, stdin, stderr to RasPi
int raspiPutChar(char c, FILE* stream);

#endif /* utils_h */