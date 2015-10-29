//
//  XboxModel.m
//  Drone Control
//
//  Created by Ludger Heide on 28.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#import "XboxModel.h"
#import "Xbox360ControllerManager/Xbox360ControllerManager.h"
#import "Xbox360ControllerManager/Xbox360Controller.h"

#define MAX_ELEVATOR 1.0
#define STEP_ELEVATOR_TRIM 0.1

#define MAX_RUDDER 1.0
#define STEP_RUDDER_TRIM 0.1

#define MAX_TRIM_VIBRATION_DURATION 0.33

#define TRIM_RANGE 0.5

@implementation XboxModel
{
    Xbox360Controller* theController;
    double elevatorTrim;
    double rudderTrim;
}

-(id) init {
    self = [super init];
    if(self) {
        Xbox360ControllerManager* sharedManager = [Xbox360ControllerManager sharedInstance];
        [sharedManager updateControllers];
        
        theController = [sharedManager getController: 0];
        if(theController == nil) {
            NSLog(@"Connecting to controller faile!");
            return nil;
        }
        theController.delegate = self;
    }
    return self;
}

-(commandSet) getValues {
    //Get the value from the correct sticks and triggers in a normalized format (-1.0 to 1.0)
    commandSet cs;
    
    //First, Pitch from the right y-axis
    double pitch = theController.rightStickY + (TRIM_RANGE * elevatorTrim);
    if(pitch > MAX_ELEVATOR) {
        pitch = MAX_ELEVATOR;
    } else if (pitch < -MAX_ELEVATOR) {
        pitch = -MAX_ELEVATOR;
    }
    cs.elevator = pitch;
    
    double rudder = theController.rightStickX + (TRIM_RANGE * rudderTrim);
    if(rudder > MAX_RUDDER) {
        rudder = MAX_RUDDER;
    } else if (rudder < -MAX_RUDDER) {
        rudder = -MAX_RUDDER;
    }
    cs.rudder = rudder;
    
    //Now, take the thrust
    double thrust = theController.rightTrigger;
    if(thrust > 1.0) {
        thrust = 1.0;
    } else if (thrust < -0.0) {
        thrust = 0.0;
    }
    cs.thrust = thrust;
    
    return cs;
}

//Delegate methods
// Digipad up button events
-(void)buttonUpPressed {
    if(elevatorTrim - STEP_ELEVATOR_TRIM > -MAX_ELEVATOR) {
        elevatorTrim -= STEP_ELEVATOR_TRIM;
    } else {
        [theController runMotorsLarge: 0 Small: 100];
        NSTimer* shutOffTimer = [NSTimer scheduledTimerWithTimeInterval: MAX_TRIM_VIBRATION_DURATION target: self selector: @selector(stopMotors:) userInfo: nil repeats:NO];
    }
}

// Digipad down button events
//The down button corresponds to pushing the trim "UP"
-(void)buttonDownPressed {
    if(elevatorTrim + STEP_ELEVATOR_TRIM < MAX_ELEVATOR) {
        elevatorTrim += STEP_ELEVATOR_TRIM;
    } else {
        [theController runMotorsLarge: 0 Small: 100];
        NSTimer* shutOffTimer = [NSTimer scheduledTimerWithTimeInterval: MAX_TRIM_VIBRATION_DURATION target: self selector: @selector(stopMotors:) userInfo: nil repeats:NO];
    }
}

// Digipad left button events
-(void)buttonLeftPressed {
    if(rudderTrim - STEP_RUDDER_TRIM > -MAX_RUDDER) {
        rudderTrim -= STEP_RUDDER_TRIM;
    } else {
        [theController runMotorsLarge: 0 Small: 100];
        NSTimer* shutOffTimer = [NSTimer scheduledTimerWithTimeInterval: MAX_TRIM_VIBRATION_DURATION target: self selector: @selector(stopMotors:) userInfo: nil repeats:NO];
    }
}

// Digipad right button events
-(void)buttonRightPressed {
    if(rudderTrim + STEP_RUDDER_TRIM < MAX_RUDDER) {
        rudderTrim += STEP_RUDDER_TRIM;
    } else {
        [theController runMotorsLarge: 0 Small: 100];
        NSTimer* shutOffTimer = [NSTimer scheduledTimerWithTimeInterval: MAX_TRIM_VIBRATION_DURATION target: self selector: @selector(stopMotors:) userInfo: nil repeats:NO];
    }
}

//Timer callback to stop motors
-(void) stopMotors: (NSTimer*) timer {
    [theController runMotorsLarge: 0 Small: 0];
}

@end
