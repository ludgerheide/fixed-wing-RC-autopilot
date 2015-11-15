//
//  CommsModel.h
//  Drone Control
//
//  Created by Ludger Heide on 18.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "ORSSerial/ORSSerialPort.h"

@protocol attitudeDelegate <NSObject>
@required
-(void) attituteChangedToCourse: (NSNumber*) course pitch: (NSNumber*) pitch roll: (NSNumber*) roll;
@end

@protocol positionDelegate <NSObject>
@required
-(void) positionChangedToLatitude: (NSNumber*) latitude longitude: (NSNumber*) longitude altitude: (NSNumber*) altitude courseOverGround: (NSNumber*) courseOverGround speed: (NSNumber*) speed;
@end

@protocol controllerDelegate <NSObject>
@required
-(void) controllerChangedWithPitch: (NSNumber*) pitch yaw: (NSNumber*) yaw thrust: (NSNumber*) thrust;
@end

@protocol batteryDelegate <NSObject>
@required
- (void) batteryChangedToVoltage: (NSNumber*) voltage current: (NSNumber*) current;
@end

@interface CommsModel : NSObject <ORSSerialPortDelegate>

@property (weak) id<attitudeDelegate> attitudeDelegate;
@property (weak) id<positionDelegate> positionDelegate;
@property (weak) id<controllerDelegate> controllerDelegate;
@property (weak) id<batteryDelegate> batteryDelegate;

//This method invalidates the Attitude and map view when the timeout is exceeded
- (void) timeOutExceeded:(NSTimer*) theTimer;

//This method collects a sample from the XBox controller and sends
- (void) sendControllerSample: (NSTimer*) theTimer;

//This method sends the sea level pressure to the plane
-(void) sendSeaLevelPressure: (NSNumber*) thePressure;
@end
