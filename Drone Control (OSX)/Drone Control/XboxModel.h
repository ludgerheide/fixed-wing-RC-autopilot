//
//  XboxModel.h
//  Drone Control
//
//  Created by Ludger Heide on 28.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "Xbox360ControllerManager/Xbox360ControllerDelegate.h"

//The struct that contains elevator, rudder and thrust
typedef struct {
    double elevator;
    double rudder;
    double thrust;
} commandSet;

@interface XboxModel : NSObject <Xbox360ControllerDelegate>

-(commandSet) getValues;

//Delegate methods
// Digipad up button events
-(void)buttonUpPressed;

// Digipad down button events
-(void)buttonDownPressed;

// Digipad left button events
-(void)buttonLeftPressed;

// Digipad right button events
-(void)buttonRightPressed;


@end
