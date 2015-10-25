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
        
        if(![myPort isOpen]) {
            NSLog(@"failed to open serial port!");
            return nil;
        }
    }
    return self;
}

//Methods to implement ORSerialDelegate
- (void)serialPort:(ORSSerialPort *)serialPort didReceiveData:(NSData *)data {
    XBeeMessage* myMsg = [[XBeeMessage alloc] initWithRawMessage: data];
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
}

-(void) serialPortWasRemovedFromSystem:(ORSSerialPort *)serialPort {
    NSAlert* serialPortAlert = [[NSAlert alloc] init];
    serialPortAlert.messageText = @"Serial port removed!";
    [serialPortAlert beginSheetModalForWindow: [[NSApp windows] firstObject] completionHandler: nil];
}

@end
