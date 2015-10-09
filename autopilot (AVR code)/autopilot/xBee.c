//
//  xBee.c
//  xBeeApi
//
//  Created by Ludger Heide on 04.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#include "xBee.h"
#include "uart4.h"
#include "pinSetup.h"
#include "communicationsHandler.h"
#include <assert.h>

#define TX_BUFFER_SIZE 128
#define RX_BUFFER_SIZE 128 //TODO: identify min size

#define TX_NONDATA_SIZE 14

#ifdef MAC
const char destinationAddress[8] = {0x00, 0x13, 0xA2, 0x00, 0x40, 0xA3, 0x23, 0x9D};
#else
const char destinationAddress[8] = {0x00, 0x13, 0xA2, 0x00, 0x40, 0xA3, 0x23, 0x82};
#endif

uint8_t received_index = 0;
uint16_t received_size = 0;

//The RX and ack handlers
typedef uint8_t (*u08FuncPtr)(char* buffer, uint8_t numBytes);
static u08FuncPtr serialTxFunc;
typedef void (*voidFuncPtr)(uint8_t frameID);
static voidFuncPtr ackFunc;
//The handler for a transmit status
typedef void (*txFuncPtr)(uint8_t frameID, uint8_t retryCount, uint8_t txStatus);
static txFuncPtr txStatusFunc;

//The TX and RX buffers
static char xBeeTxBuffer[TX_BUFFER_SIZE];
static char xBeeRxBuffer[RX_BUFFER_SIZE];

bool xBeeNewMessageReady = false;

//FUnction prototypes
//Calaculates the checksum of a xBee API message
static uint8_t xBeeGenerateChecksum(char* msg, uint8_t size);

void xBeeInit(void) {
    serialTxFunc = 0;
    ackFunc = 0;
    txStatusFunc = 0;
    
    //Initialize the xbee uart
    uartInit(XBEE_UART);
    uartSetBaudRate(XBEE_UART, 115200);
    
    uartSetRxHandler(XBEE_UART, xBeeByteReceiver );
}

//Sends out a buffer using the xBee api
//shouldAck indicets whether the xBee should retransmit up to 10 times if the message is not acknowledged
void xBeeSendPayload(char* payload, uint8_t bufferSize, bool shouldAck, uint8_t frameID) {
    assert(bufferSize <= 100);
    
    //Set the first byte of the buffer to 0x7E, the magic start number
    xBeeTxBuffer[0] = 0x7E;
    
    //Add the size of address etc for the size we send to the xbee
    uint16_t sizeForMessage = bufferSize + TX_NONDATA_SIZE;
    
    xBeeTxBuffer[1] = sizeForMessage << 8; //MSB goes here
    xBeeTxBuffer[2] = sizeForMessage; //LSB goes here
    
    //Now set the frame type to 0x10
    xBeeTxBuffer[3] = 0x10;
    
    //Now the frame ID. If it is 0, no ACK will be sent
    xBeeTxBuffer[4] = frameID;
    
    //Now the destination address
    for(uint8_t i = 0; i < 8; i++) {
        xBeeTxBuffer[5+i] = destinationAddress[i];
    };
    
    // reserved (0xFFFE
    xBeeTxBuffer[13] = 0xFF;
    xBeeTxBuffer[14] = 0xFE;
    
    //Broadcast radius
    xBeeTxBuffer[15] = 0x00;
    
    //Transmit options, bit 0 indicates if the remote station should ACK
    if(shouldAck) {
        xBeeTxBuffer[16] = 0b00000000;
    } else {
        xBeeTxBuffer[16] = 0b00000001;
    }
    
    //Now the RF payload
    for (uint8_t i = 0; i < bufferSize; i++) {
        xBeeTxBuffer[17 + i] = payload[i];
    }
    
    xBeeTxBuffer[17 + bufferSize] = xBeeGenerateChecksum(&xBeeTxBuffer[3], TX_NONDATA_SIZE + bufferSize);
    
    for(u08 i = 0; i < bufferSize + 18; i++) {
        printf("%02x ", xBeeTxBuffer[i]);
    }
    printf("\r\n");
    
    //Now the payload is complete. Send it out over the serial port
    uint8_t result = uartSendBuffer(XBEE_UART, xBeeTxBuffer, bufferSize + 18);
    
    printf("uart: %i, size: %i, result: %i\r\n", XBEE_UART, bufferSize + 18, result);
    
    if(result != FALSE) {
        return;
    }
    else {
        #ifdef COMMS_DEBUG
        printf("Error Sending!\r\n");
        #endif
    }
}

//Calaculates the checksum of a xBee API message
static uint8_t xBeeGenerateChecksum(char* msg, uint8_t size) {
    uint8_t checksum = 0x00;
    for(uint8_t i = 0; i < size; i++) {
        checksum += *(msg + i);
    }
    return 0xFF - checksum;
}

//Called if a byte is received over the serial port. Checks if it is a start byte and the fills th ebuffer accordingly
void xBeeByteReceiver(unsigned char c) {
    #ifdef COMMS_DEBUG
    printf(" %02x",c);
    #endif
    if(received_index == 0) {
        //We are looking for the start byte, 0x7E
        if(c == 0x7E) {
            ;
        } else {
            return;
        }
    } else if (received_index == 1) {
        //This is the msb of the size
        received_size = c << 8;
    } else if (received_index == 2) {
        //This is the LSB of the size
        received_size |= c;
    } else if (received_index < received_size + 3 && (received_index - 3) < RX_BUFFER_SIZE) {
        xBeeRxBuffer[received_index - 3] = c;
    } else if (received_index == received_size + 3) {
        uint8_t checksum = xBeeGenerateChecksum(xBeeRxBuffer, received_size);
        if(checksum == c) {
            //We have recieved a correnct message, set the "Ready" flag
            xBeeNewMessageReady = true;
            #ifdef COMMS_DEBUG
            printf("\r\n");
            #endif
            //TODO: Assert the RTS (?) line so no more data gets sent until we dealt with this data
        } else {
            received_size = 0;
            received_index = 0;
        }
    } else {
        printf("Buffer overflow!");
    }
    received_index++;
}

//Handles a completed xBee buffer
void xBeeHandleMessage(void) {
#define XBEE_MSGTYPE_TXSTATUS 0x8B
#define XBEE_MSGTYPE_RECEIVE 0x90
    
    switch (xBeeRxBuffer[0]) {
        case XBEE_MSGTYPE_RECEIVE:
        {
            char* protoBufMessage = &xBeeRxBuffer[12];
            uint8_t protoBufSize = received_size - 12;
            commsProcessMessage(protoBufMessage, protoBufSize);
        }
            break;
            
        case XBEE_MSGTYPE_TXSTATUS:
        {
            if(txStatusFunc) {
                txStatusFunc(xBeeRxBuffer[1], xBeeRxBuffer[4], xBeeRxBuffer[5]);
            }
            break;
        }
            
        default:
            #ifdef COMMS_DEBUG
            printf("Other packet type received!");
            #endif
            break;
    }
    //Reset the received index and "new message ready" flag
    received_index = 0;
    received_size = 0;
    xBeeNewMessageReady = false;
}

//Attaches the "send buffer" function to send the completed message out.
// The sendbuffer function must be of the format
// void sendBufferFunc(char *buffer, uint8_t nBytes)
void xBeeAttachSendFunction(uint8_t (*tx_func)(char* buffer, uint8_t numBytes)) {
    serialTxFunc = tx_func;
}

//Attaches the acknowgledgement handler function
void xBeeAttachAckFunction(void (*ack_func)(uint8_t frameID)) {
    ackFunc = ack_func;
}

void xBeeAttachTxStatusHandler(void (*stat_func)(uint8_t frameID, uint8_t retryCount, uint8_t txStatus)) {
    txStatusFunc = stat_func;
}