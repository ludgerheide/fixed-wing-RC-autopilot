//
//  xBee.h
//  xBeeApi
//
//  Created by Ludger Heide on 04.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#ifndef xBee_h
#define xBee_h

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

extern bool xBeeNewMessageReady;

//Initializes the xBee serial port and attaches the callbacks
void xBeeInit(void);

//Sends out a buffer using the xBee api
//shouldAck indicets whether the xBee should retransmit up to 10 times if the message is not acknowledged
void xBeeSendPayload(char* payload, uint8_t bufferSize, bool shouldAck, uint8_t frameID);

//Called if a byte is received over the serial port. Checks if it is a start byte and the fills th ebuffer accordingly
void xBeeByteReceiver(unsigned char c);

//Handles a completed xBee buffer
void xBeeHandleMessage(void);

//Attaches the "send buffer" function to send the completed message out.
// The sendbuffer function must be of the format
// void sendBufferFunc(char *buffer, uint8_t nBytes)
void xBeeAttachSendFunction(uint8_t (*tx_func)(char* buffer, uint8_t numBytes));

//Attaches the functiont hat gets called when we receive a "transmit status"
void xBeeAttachTxStatusHandler(void (*stat_func)(uint8_t frameID, uint8_t retryCount, uint8_t txStatus));

#endif /* xBee_h */
