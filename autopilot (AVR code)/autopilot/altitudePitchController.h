//
//  altitudePitchController.h
//  autopilot
//
//  Created by Ludger Heide on 13.12.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#ifndef altitudePitchController_h
#define altitudePitchController_h

#include "avrlibtypes.h"

//Calculate the pitch angle for a given target altitude
//Using a PI controller
//INPUT: signed 32-Bit value indicating the target altitude in centimeters
//OUTPUT: signed 8-Bit value indicating the desired pitch angle
s08 calculatePitchAngle(s32 targetAltitude);

//Calculate the rudder for a given pitch angle using a PID controller
//INPUT: signed 8-bit value indicating the desired pitch angle
//OUTPUR: *unsigned* 8-bit value to go to the servo
u08 calculateElevatorValue(s08 targetPitchAngle);

#endif /* altitudePitchController_h */
