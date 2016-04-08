//
//  yawController.c
//  autopilot
//
//  Created by Ludger Heide on 17.12.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#include "yawController.h"
#include "global.h"
#include "utils.h"
#include <math.h>
#include <assert.h>

#define DEVIATION_FOR_MAXIMUM_RATE_OF_TURN 45
#define COMPASS_DECLINATION 0 //add this to magentic north to get true north (Basically 0 in western europe)
#define MAX_YAW_ROTATION (M_PI/8.0) //The maximum rate of turn we want to allow (in rad/s)
#define MAX_ROLL_ROTATION (M_PI/8.0) //The roll rate that is opposed by a maximum deflection
#define MAX_BANK_ANGLE 30.0 //The bank angle that correspons to our maximum turning speed

#define BANK_ANGLE_GAIN 0.66
#define YAW_RATE_GAIN 0.33

//Calculates the turning speed for a given target course
//INPUT: Magnetic course in degrees (0-360)
//OUTPUT: Value between -127 and 127 indicating rate of turn. Positive indicates a turn to the right (clockwise if viewed from top)
s08 calculateRateOfTurn(s16 wantedCourse) {
    s16 currentCourse = currentAttitude.courseMagnetic + COMPASS_DECLINATION;
    //Wonky angle subtraction
    s16 difference = wantedCourse - currentCourse;
    if (abs(difference) > 180) {
        if (wantedCourse - currentCourse < 0) {
            wantedCourse += 360;
        } else {
            currentCourse += 360;
        }
    }
    difference = wantedCourse - currentCourse;
    
    assert(abs(difference) <= 180);
    
    s08 rateOfTurn = maps16(difference, -DEVIATION_FOR_MAXIMUM_RATE_OF_TURN, DEVIATION_FOR_MAXIMUM_RATE_OF_TURN, INT8_MIN, INT8_MAX);
    return rateOfTurn;
}

//Calculate the rudder for a given pitch angle using a PID controller
//INPUT: signed 8-bit value indicating the desired rate of turn (positive means right)
//OUTPUT: *unsigned* 8-bit value to go to the servo
u08 calculateRudderValue(s08 targetRateOfTurn) {
    float yawRate = curGyro.z; //Positive value means turning to the right (clockwise), negative left (counteclockwise)
    s08 bankAngle = currentAttitude.roll; //Positive value means dipping the right wing (clockwise viewed from behind the aircraft), negative means dipping left wing
    
    //Interpret the input value as rotation
    float wantedYawRate = mapfloat(targetRateOfTurn, INT8_MIN, INT8_MAX, -MAX_YAW_ROTATION, MAX_YAW_ROTATION);
    float yawDifference = wantedYawRate - yawRate;
    float normalizedYawDifference = mapfloat(yawDifference, -MAX_YAW_ROTATION, MAX_YAW_ROTATION, -1, 1);
    
    //Calculate the bank angle difference
    float wantedBankAngle = mapfloat(targetRateOfTurn, INT8_MIN, INT8_MAX, -MAX_BANK_ANGLE, MAX_BANK_ANGLE);
    float bankAngleDifference = wantedBankAngle - bankAngle;
    float normalizedBankAngleDifference = mapfloat(bankAngleDifference, -MAX_BANK_ANGLE, MAX_BANK_ANGLE, -1, 1);
    
    //Add the two together with respective coeffiecents
    float normalizedRudder = YAW_RATE_GAIN * normalizedYawDifference + BANK_ANGLE_GAIN * normalizedBankAngleDifference;
    return mapfloat(normalizedRudder, 1, -1, 0, UINT8_MAX);
}