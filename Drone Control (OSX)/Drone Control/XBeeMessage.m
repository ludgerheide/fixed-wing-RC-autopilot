//
//  XBeeMessage.m
//  test
//
//  Created by Ludger Heide on 18.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import "XBeeMessage.h"

#define NONDATA_SIZE 14
#define NONDATA_SIZE_INCLUDING_HEADER 18

#define FRAME_ID_INDEX 4

#define ACKFLAG_INDEX 16
#define ACKFLAG_YES 0b00000000
#define ACKFLAG_NO 0b00000001

//Private vars

@implementation XBeeMessage

//Setters/getters
@synthesize payloadData;
@synthesize shouldAck;
@synthesize frameID;
@synthesize delegate;

//Initializers
-(XBeeMessage*) initWithPayload: (NSData*) payload {
    //We only support a payload lower than 100 bytea
    if(payload.length >= 100) {
        return nil;
    }
    
    self = [super init];
    
    if(self) {
        shouldAck = NO;
        frameID = 0;
        payloadData = payload;
    }
    return self;
}

-(XBeeMessage*) initWithRawMessage: (NSData*) rawMessage {
    self = [super init];
    
    if(self) {
        uint8_t firstThreeBytes[3];
        [rawMessage getBytes: &firstThreeBytes length: 3];
        
        uint16_t length = 0;
        if(firstThreeBytes[0] == 0x7E) {
            length = firstThreeBytes[1];
            length |= firstThreeBytes[2];
        }
        
        if(length == rawMessage.length - 4) {
            [self decodeMessage: rawMessage];
            if(!payloadData) {
                return nil;
            }
        } else {
            return nil;
        }
    }
    return self;
}

-(void) decodeMessage:(NSData *)rawMessage {
    const unsigned char* msgBytes = rawMessage.bytes;
    
#define XBEE_MSGTYPE_TXSTATUS 0x8B
#define XBEE_MSGTYPE_RECEIVE 0x90
    
    switch (msgBytes[3]) {
        case XBEE_MSGTYPE_RECEIVE:
        {
            char* protoBufMessage = (char*)&msgBytes[15];
            uint8_t protoBufSize = rawMessage.length - 16;
            payloadData = [NSData dataWithBytes: protoBufMessage length: protoBufSize];
        }
            break;
            
        case XBEE_MSGTYPE_TXSTATUS:
        {
            if(delegate) {
                NSLog(@"Ack receeved for frame ID %02x, retryCount %02x, txSTatus %02x", msgBytes[4], msgBytes[7], msgBytes[8]);
                [delegate didReceiveTransmitStatusWithFrameID: msgBytes[4] retryCount: msgBytes[7] txStatus: msgBytes[8]];
            }
            break;
        }
            
        default:
#ifdef COMMS_DEBUG
            printf("Other packet type received!");
#endif
            break;
    }
}

-(NSData*) encodeMessage {
    uint8_t* rawBytes = malloc(payloadData.length + NONDATA_SIZE_INCLUDING_HEADER);
    if(rawBytes == NULL) {
        return nil;
    }
    
    //Set the first byte of the buffer to 0x7E, the magic start number
    rawBytes[0] = 0x7E;
    
    //Add the size of address etc for the size we send to the xbee
    uint16_t sizeForMessage = payloadData.length + NONDATA_SIZE;
    
    rawBytes[1] = sizeForMessage << 8; //MSB goes here
    rawBytes[2] = sizeForMessage; //LSB goes here
    
    //Now set the frame type to 0x10
    rawBytes[3] = 0x10;
    
    //Now the frame ID. If it is 0, no ACK will be sent
    rawBytes[4] = frameID;
    
    //Now the destination address
    const char destinationAddress[8] = {0x00, 0x13, 0xA2, 0x00, 0x40, 0xA3, 0x23, 0x9D};
    //If we swap modules, thsi will bve the destination address
    //const char destinationAddress[8] = {0x00, 0x13, 0xA2, 0x00, 0x40, 0xA3, 0x23, 0x82};
    for(uint8_t i = 0; i < 8; i++) {
        rawBytes[5+i] = destinationAddress[i];
    };
    
    // reserved (0xFFFE=
    rawBytes[13] = 0xFF;
    rawBytes[14] = 0xFE;
    
    //Broadcast radius
    rawBytes[15] = 0x00;
    
    //Transmit options, bit 0 indicates if the remote station should ACK
    if(shouldAck) {
        rawBytes[16] = ACKFLAG_YES;
    } else {
        rawBytes[16] = ACKFLAG_NO;
    }
    
    //Now the RF payload
    const char* payloadBytes = [payloadData bytes];
    
    for (uint8_t i = 0; i < payloadData.length; i++) {
        rawBytes[17 + i] = payloadBytes[i];
    }
    
    rawBytes[17 + payloadData.length] = [XBeeMessage calculateChecksum: &rawBytes[3] forSize: (NONDATA_SIZE + payloadData.length)];
    
    for(uint8_t i = 0; i < payloadData.length + NONDATA_SIZE_INCLUDING_HEADER; i++) {
        printf("%02x ", rawBytes[i]);
    }
    printf("\r\n");
    
    //Now the C stuff is done and we go back into an objective-C Object
    NSData* rawData = [NSData dataWithBytes: rawBytes length: payloadData.length + NONDATA_SIZE_INCLUDING_HEADER];
    free(rawBytes);
    return rawData;
}



+(uint8_t) calculateChecksum: (uint8_t*) msg forSize: (uint8_t) size {
    uint8_t checksum = 0x00;
    for(uint8_t i = 0; i < size; i++) {
        checksum += *(msg + i);
    }
    return 0xFF - checksum;
}

@end
