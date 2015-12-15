//
//  CommsModel.m
//  Drone Control
//
//  Created by Ludger Heide on 18.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#define POLLING_INTERVAL 0.05 //20hz

#define INSTRUMENTTIMEOUT 2

#import "CommsModel.h"
#import "XBeeMessage.h"
#import "CommunicationProtocol.pbobjc.h"
#import "XboxModel.h"
#import <Cocoa/Cocoa.h>

@implementation CommsModel
{
    ORSSerialPort* myPort;
    
    NSMutableData* receivedData;
    
    XboxModel* controllerModel;
    
    NSTimer* instrumentTimeoutTimer;
    NSTimer* controllerPollTimer;
}

@synthesize attitudeDelegate;
@synthesize positionDelegate;
@synthesize controllerDelegate;
@synthesize batteryDelegate;


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
        
        //Now, the controller
        controllerModel = [[XboxModel alloc] init];
        
        if(controllerModel) {
            //Schedule the repeating timer for the controller
            controllerPollTimer = [NSTimer scheduledTimerWithTimeInterval: POLLING_INTERVAL target: self selector: @selector(sendControllerSample:) userInfo:nil repeats: YES];
        } else {
            
        }
    }
    return self;
}

-(void) sendSeaLevelPressure: (NSNumber*) thePressure {
    if(thePressure.doubleValue <= 1100 && thePressure.doubleValue >= 850) {
        //We have a valid pressure, create a message containing it
        //Create a protobuf with this stuff and send it
        DroneMessage* msg = [[DroneMessage alloc] init];
        msg.seaLevelPressure = thePressure.doubleValue;
        
        XBeeMessage* xBeeMsg = [[XBeeMessage alloc] initWithPayload: msg.data];
        
        xBeeMsg.shouldAck = true;
        xBeeMsg.frameID = 0xFE;
        
        [myPort sendData: xBeeMsg.encodeMessage];

    }
}

//This method invalidates the Attitude and map view when the timeout is exceeded
- (void) timeOutExceeded:(NSTimer*) theTimer {
    //Send a nil attitude to the delegates
    [attitudeDelegate attituteChangedToCourse: nil
                                        pitch: nil
                                         roll: nil];
    [positionDelegate positionChangedToLatitude: nil longitude: nil courseOverGround: nil speed: nil];
    [positionDelegate altitudeChanged: nil];
}

//This method collects a sample from the XBox controller and sends
- (void) sendControllerSample: (NSTimer*) theTimer {
    commandSet cs = [controllerModel getValues];
    
    NSNumber* pitch = [NSNumber numberWithDouble: cs.elevator];
    NSNumber* yaw = [NSNumber numberWithDouble: cs.rudder];
    NSNumber* thrust = [NSNumber numberWithDouble: cs.thrust];
    
    if(controllerDelegate) {
        [controllerDelegate controllerChangedWithPitch: pitch yaw: yaw thrust: thrust];
    }
    
    //Create a protobuf with this stuff and send it
    DroneMessage* msg = [[DroneMessage alloc] init];
    
    const uint8_t servo_max = 255;
    
    msg.inputCommandSet.pitch = round((cs.elevator + 1.0) * (servo_max / 2.0));
    msg.inputCommandSet.yaw = round((cs.rudder + 1.0) * (servo_max / 2.0));
    msg.inputCommandSet.thrust = round(cs.thrust * servo_max);
    
    XBeeMessage* xBeeMsg = [[XBeeMessage alloc] initWithPayload: msg.data];
    
    xBeeMsg.shouldAck = false;
    xBeeMsg.frameID = 0x00;
    
    [myPort sendData: xBeeMsg.encodeMessage];
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
                    
                    //Dlete the old timeout timer and start a new one
                    if(instrumentTimeoutTimer) {
                        [instrumentTimeoutTimer invalidate];
                    }
                    instrumentTimeoutTimer = [NSTimer scheduledTimerWithTimeInterval: INSTRUMENTTIMEOUT target: self selector: @selector(timeOutExceeded:) userInfo: nil repeats: NO];
                    instrumentTimeoutTimer.tolerance = 0.5;
                }
            } else {
                NSLog(@"Invalid message (no attitude)");
            }
            
            if(myDecodedMsg.hasCurrentPosition) {
                double latitude = myDecodedMsg.currentPosition.latitude;
                double longitude = myDecodedMsg.currentPosition.longitude;
                double speed = myDecodedMsg.currentSpeed.speed / 1000.0;
                double courseOverGround = myDecodedMsg.currentSpeed.courseOverGround/64.0;
                if(positionDelegate) {
                    [positionDelegate positionChangedToLatitude: [NSNumber numberWithDouble: latitude]
                                                      longitude: [NSNumber numberWithDouble: longitude]
                                               courseOverGround: [NSNumber numberWithDouble: courseOverGround]
                                                          speed: [NSNumber numberWithDouble: speed]];
                }
            }
            
            if(myDecodedMsg.hasCurrentAltitude) {
                double altitude = myDecodedMsg.currentAltitude / 100.0;
                if(positionDelegate) {
                    [positionDelegate altitudeChanged: [NSNumber numberWithDouble: altitude]];
                }
            }
            
            if(myDecodedMsg.hasCurrentBatteryData) {
                double voltage = myDecodedMsg.currentBatteryData.voltage / 1000.0;
                double current = myDecodedMsg.currentBatteryData.current / 1000.0;
                
                if(batteryDelegate) {
                    [batteryDelegate batteryChangedToVoltage: [NSNumber numberWithDouble: voltage]
                                                     current: [NSNumber numberWithDouble: current]];
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
