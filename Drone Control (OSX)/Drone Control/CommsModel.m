//
//  CommsModel.m
//  Drone Control
//
//  Created by Ludger Heide on 18.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import "CommsModel.h"
#import "XBeeMessage.h"
#import "CommunicationProtocol.pbobjc.h"
#import <Cocoa/Cocoa.h>

@implementation CommsModel
{
    ORSSerialPort* myPort;
    NSMutableData* receivedData;
}

@synthesize attitudeDelegate;
@synthesize positionDelegate;


- (CommsModel*) init {
    self = [super init];
    
    if(self) {
        myPort = [ORSSerialPort serialPortWithPath: @"/dev/tty.usbserial-DA017KGT"];
        myPort.baudRate = [NSNumber numberWithInteger: 115200];
        myPort.delegate = self;
        [myPort open];
        
        receivedData = [[NSMutableData alloc] init];
        
        if(![myPort isOpen]) {
            NSLog(@"failed to open serial port!");
            return nil;
        }
    }
    return self;
}

//Methods to implement ORSerialDelegate
- (void)serialPort:(ORSSerialPort *)serialPort didReceiveData:(NSData *)data {
    //append the new data
    [receivedData appendData: data];
    
    //Discard the front until it is 0x7e
    uint8_t firstByte = 0x00;
    [receivedData getBytes: &firstByte length: 1];
    while (firstByte != 0x7e && receivedData.length > 1) {
        const uint8_t* receivedBytes = receivedData.bytes;
        NSMutableData* newData = [NSMutableData dataWithBytes: (receivedBytes + 1) length: receivedData.length - 1];
        receivedData = newData;
        [receivedData getBytes: &firstByte length: 1];
    }
    if(receivedData.length < 3) {
        return;
    }
    
    //Now the first byte is definately 0x7e. Check the size, if we have the complete message process it and remove it form the buffer
    uint8_t recSizeLSB;
    uint8_t recSizeMSB;
    [receivedData getBytes: &recSizeMSB range: NSMakeRange(1, 1)];
    [receivedData getBytes: &recSizeLSB range: NSMakeRange(2, 1)];
    
    uint16_t recSize = (recSizeMSB << 8) | recSizeLSB;
    
    if(receivedData.length > recSize + 4) { //Three bytes header + one byte checksum
        NSData* packet = [NSData dataWithBytes: receivedData.bytes length: recSize + 4];
        
        NSMutableData* newData = [NSMutableData dataWithBytes: (receivedData.bytes + recSize + 4) length: receivedData.length - (recSize + 4)];
        receivedData = newData;
        
        XBeeMessage* myMsg = [[XBeeMessage alloc] initWithRawMessage: packet];
        if(myMsg != nil) {
            NSError* myError;
            DroneMessage* myDecodedMsg = [DroneMessage parseFromData: myMsg.payloadData error: &myError];
            
            if(myDecodedMsg.hasCurrentAttitude) {
                double heading = myDecodedMsg.currentAttitude.courseMagnetic/64.0;
                double pitch = myDecodedMsg.currentAttitude.pitch/64.0;
                double roll = myDecodedMsg.currentAttitude.roll/64.0;
                if(attitudeDelegate) {
                    [attitudeDelegate attituteChangedToCourse: [NSNumber numberWithDouble: heading]
                                                        pitch: [NSNumber numberWithDouble: pitch]
                                                         roll: [NSNumber numberWithDouble: roll]];
                }
            } else {
                NSLog(@"Invalid message (no attitude)");
            }
            
            if(myDecodedMsg.hasCurrentPosition) {
                double latitude = myDecodedMsg.currentPosition.latitude;
                double longitude = myDecodedMsg.currentPosition.longitude;
                double altitude = myDecodedMsg.currentPosition.altitude / 100.0;
                
                double speed = myDecodedMsg.currentSpeed.speed / 1000.0;
                double courseOverGround = myDecodedMsg.currentSpeed.courseOverGround/64.0;
                if(positionDelegate) {
                    [positionDelegate positionChangedToLatitude: [NSNumber numberWithDouble: latitude]
                                                      longitude: [NSNumber numberWithDouble: longitude]
                                                       altitude: [NSNumber numberWithDouble: altitude]
                                               courseOverGround: [NSNumber numberWithDouble: courseOverGround]
                                                          speed: [NSNumber numberWithDouble: speed]];
                }
            }
        }
        
        //If we still have a valid message, recurse
        if(receivedData.length >= 3) {
            uint8_t recSizeLSB;
            uint8_t recSizeMSB;
            [receivedData getBytes: &recSizeMSB range: NSMakeRange(1, 1)];
            [receivedData getBytes: &recSizeLSB range: NSMakeRange(2, 1)];
            
            uint16_t recSize = (recSizeMSB << 8) | recSizeLSB;
            
            if(receivedData.length > recSize + 4) {
                [self serialPort: serialPort didReceiveData: [NSData dataWithBytes: NULL length: 0]];
            }
        }
    }
    
    
    
}

-(void) serialPortWasRemovedFromSystem:(ORSSerialPort *)serialPort {
    NSAlert* serialPortAlert = [[NSAlert alloc] init];
    serialPortAlert.messageText = @"Serial port removed!";
    [serialPortAlert beginSheetModalForWindow: [[NSApp windows] firstObject] completionHandler: nil];
}

@end