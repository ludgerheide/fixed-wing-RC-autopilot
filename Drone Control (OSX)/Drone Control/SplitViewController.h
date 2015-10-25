//
//  SplitViewController.h
//  Drone Control
//
//  Created by Ludger Heide on 16.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "CommsModel.h"

@interface SplitViewController : NSSplitViewController <attitudeDelegate, positionDelegate>

@end
