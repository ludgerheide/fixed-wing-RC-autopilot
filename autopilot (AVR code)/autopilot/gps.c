//
//  gps.c
//  avr - flyByWire
//
//  Created by Ludger Heide on 01.10.15.
//  Copyright © 2015 LH Technologies. All rights reserved.
//

#include "uart4.h"

#include "gps.h"
#include "pinSetup.h"
#include "utils.h"
#include "buffer.h"

#include <assert.h>
#include <util/delay.h>
#include <avr/io.h>


static u08 newlineCount = 0;
static u08 lastMessageReceived;
const u08 numberOfSentences = 2;

//Sets up the GPS Serial port, sends the initialization commands to the GPS and switches the baud rate
void gpsInit(void) {
    //Initialize UART for the GPS, set the baudrate to the initial baud rate
    uartInit(GPS_UART);
    uartSetBaudRate(GPS_UART, 9600);
    
    const char* baudrateCommand = "$PMTK251,115200*1F\r\n";
    uartPutStr(GPS_UART, (char*)baudrateCommand);
    
    //Delay to let it settele everything
    _delay_ms(10);
    
    //Switch to the higher baudrate and send the sentence configuration after the uart is ready
    while (!uartReadyTx[GPS_UART]) {
        ;
    }
    uartSetBaudRate(GPS_UART, 115200);
    
    const char* sentenceCommand = "$PMTK314,0,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0*28\r\n";
    uartPutStr(GPS_UART, (char*)sentenceCommand);
    
    //Set teh update-rate to 1Hz, 1000ms between sentences (update to 5hz later)
    //const char* updateRateCommand = "$PMTK220,1000*1F\r\n";
    //const char* updateRateCommand = "$PMTK220,200*2C\r\n";
    const char* updateRateCommand = "$PMTK220,500*2B\r\n";
    
    uartPutStr(GPS_UART, (char*)updateRateCommand);
    
    //Attach our hanlder to the serial port
    uartSetRxHandler(GPS_UART, gpsRxHandler );
    
    //Wait for the replies and delete them
    bufferFlush(uartGetRxBuffer(GPS_UART));
}

//Checks if the GPS_UART buffer is ready for an NMEA update
BOOL gpsCheck(void) {
    if(newlineCount >= numberOfSentences) {
        return TRUE;
    } else {
        return FALSE;
    }
}

//Calls the NMEA parser and updates the struct
void gpsUpdate(void) {
    assert(newlineCount >= numberOfSentences);
    
    //We have to call process() twice, once for the GGA, once for the VTG
    nmeaProcess(uartGetRxBuffer(GPS_UART));
    
    lastMessageReceived = nmeaProcess(uartGetRxBuffer(GPS_UART));
    
    bufferFlush(uartGetRxBuffer(GPS_UART));
    newlineCount = 0;
}


//This gets attached to the UART RX handler.
// It counts the '\r\n' combinations that mark the end of the packets.
// When two packets (GGA + VTG) Are in the buffer, it sets a flag so that they are parsed in the next run
void gpsRxHandler(unsigned char c) {
    cBuffer* serialRxBuffer = uartGetRxBuffer(GPS_UART);

    // put received char in buffer
    // check if there's space
    if( !bufferAddToEnd(serialRxBuffer, c) ) {
        #ifdef GPS_DEBUG
        printf("Buffer full!\r\n");
        #endif
        
        //If the buffer is overflowing, clear it when we receive a new sentence (indicated by a '$' sign)
        if(c == '$') {
            bufferFlush(serialRxBuffer);
            bufferAddToEnd(serialRxBuffer, c);
        }
    }
    
    //check if the character was a '\n'
    else if(c == '\n') {
        //If we have aprtial sentence in the buffer (we started receiving after the start, discard it
        if(bufferGetAtIndex(serialRxBuffer, 0) != '$') {
            newlineCount = 0;
            bufferFlush(serialRxBuffer);
        }
        //If the last message received was a GGA, this is the end of the corresponding VTG.
        // Discard it to resynchronize
        if(lastMessageReceived == NMEA_GPGGA) {
            newlineCount = 0;
            bufferFlush(serialRxBuffer);
        } else {
            newlineCount++;
        }
    }
}