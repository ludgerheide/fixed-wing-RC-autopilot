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

//Calculates the rate of climb for a given target altitude
//INPUT: Altitude in centimeters
//OUTPUT: Value between -127 and 127 indicating climb(positive) or descend(negative)
s08 calculateRateOfClimb(s32 targetAltitude);

//Calculate the pitch angle for a given rate of climb
//Using a PI controller
//INPUT: signed 8-Bit value indicating the rate of climb
//OUTPUT: signed 8-Bit value indicating the desired pitch angle
s08 calculatePitchAngle(s08 targetRateOfClimb);

//Calculate the rudder for a given pitch angle using a PID controller
//INPUT: signed 8-bit value indicating the desired pitch angle
//OUTPUR: *unsigned* 8-bit value to go to the servo
u08 calculateElevatorValue(s08 targetPitchAngle);

#endif /* altitudePitchController_h */
