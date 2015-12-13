//
//  utils.c
//  avr - flyByWire
//
//  Created by Ludger Heide on 23.09.15.
//  Copyright © 2015 LH Technologies. All rights reserved.
//

#include "utils.h"
#include "uart4.h"

#include "pinSetup.h"

static FILE mystdout = FDEV_SETUP_STREAM(raspiPutChar, NULL, _FDEV_SETUP_WRITE);

void printfAttachToUart(void) {
    //Initialize UART 3 and make it stderr and stdout
    uartInit(RASPI_UART);
    uartSetBaudRate(RASPI_UART, 115200);
    stderr = &mystdout;
    stdout = &mystdout;
}

//Standards-compliant putchar function that sends stuff to the raspberry pi
int raspiPutChar(char c, FILE* stream) {
    uartSendByte(RASPI_UART, c);
    return 0;
}