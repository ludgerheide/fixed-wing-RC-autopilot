//
//  PFDViewController.h
//  Drone Control
//
//  Created by Ludger Heide on 13.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CommsModel.h"

@interface PFDViewController : NSViewController <controllerDelegate, batteryDelegate>;

@property (nonatomic) NSNumber* speed;
@property (nonatomic) NSNumber* altitude;
@property (nonatomic) NSNumber* pitch;
@property (nonatomic) NSNumber* roll;
@property (nonatomic) NSNumber* heading;

- (void) updateHorizon;

@end
