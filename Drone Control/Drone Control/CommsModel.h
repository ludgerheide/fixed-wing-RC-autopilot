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

@interface CommsModel : NSObject <ORSSerialPortDelegate>

@property id<attitudeDelegate> attitudeDelegate;
@property id<positionDelegate> positionDelegate;

@end
