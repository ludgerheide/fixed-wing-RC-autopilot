//
//  yawController.h
//  autopilot
//
//  Created by Ludger Heide on 17.12.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#ifndef yawController_h
#define yawController_h

#include "avrlibtypes.h"

//Calculates the turning speed for a given target course
//INPUT: Magnetic course in degrees (0-360)
//OUTPUT: Value between -127 and 127 indicating rate of turn. Positive indicates a turn to the right (clockwise if viewed from top)
s08 calculateRateOfTurn(s16 wantedCourse);

//Calculate the rudder for a given pitch angle using a PID controller
//INPUT: signed 8-bit value indicating the desired rate of turn
//OUTPUT: *unsigned* 8-bit value to go to the servo
u08 calculateRudderValue(s08 targetRateOfTurn);

#endif /* yawController_h */
