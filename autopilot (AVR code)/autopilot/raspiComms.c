//
//  raspiComms.c
//  autopilot
//
//  Created by Ludger Heide on 10.04.16.
//  Copyright © 2016 Ludger Heide. All rights reserved.
//

#include "raspiComms.h"
#include "communicationsHandler.h"
#include "uart4.h"
#include "pinSetup.h"

#define RX_BUFFER_SIZE 128 //TODO: identify min size
static char raspiRxBuffer[RX_BUFFER_SIZE];

const char startMarker[5] = {'s','t','a','r','t'};

volatile BOOL raspiNewMessageReady;
volatile u08 received_index;
volatile u08 msgSize;
volatile u08 checksum;

//Initializes comms with the RasPi
void raspiInit(void) {
    uartInit(RASPI_UART);
    uartSetBaudRate(RASPI_UART, 115200);
    
    uartSetRxHandler(RASPI_UART, raspiByteReceiver );
}

//Called each time the raspi has sent a byte. ISR method – needs to be fast
void raspiByteReceiver(u08 c) {
#ifdef COMMS_DEBUG
//    printf("%u: %02x\r\n",received_index, c);
#endif
    if(received_index < 5) {
        //For indices 0 to 4, it needs to match the "start" marker
        if(c == startMarker[received_index]) {
            received_index++;
            return;
        } else {
            received_index = 0;
            return;
        }
    } else if(received_index == 5) {
        //At index 5, we should receive the message size
        msgSize = c;
        checksum = 0;
        received_index++;
        return;
    } else if(received_index < (msgSize + 5 + 1) && (received_index - (5 + 1)) < RX_BUFFER_SIZE) {
        raspiRxBuffer[received_index - (5 + 1)] = c;
        checksum += c;
        received_index++;
        return;
    } else if(received_index == (msgSize + 5 + 1)) {
        u08 receivedChecksum = c;
        if(receivedChecksum == checksum) {
            raspiNewMessageReady = TRUE;
            received_index++;
        } else {
            received_index = 0;
        }
        return;
    } else {
#ifdef COMMS_DEBUG
        printf("Buffer overflow @%i!\r\n", __LINE__);
        for(u08 i = 0; i < msgSize; i++) {
            printf("%02x ", raspiRxBuffer[i]);
        }
        printf("Size: %u\r\n", msgSize);
#endif
        received_index = 0;
        return;
    }
}

//Called when a complete message os in the buffer
void raspiHandleMessage(void) {
#ifdef COMMS_DEBUG
    printf("Processing @%i!\r\n", __LINE__);
    for(u08 i = 0; i < msgSize; i++) {
        printf("%02x ", raspiRxBuffer[i]);
    }
    printf("Size: %u\r\n", msgSize);
#endif
    
    commsProcessMessage((char*)raspiRxBuffer, msgSize);
    
    received_index = 0;
}