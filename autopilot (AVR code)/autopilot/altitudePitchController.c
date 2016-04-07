//
//  altitudePitchController.c
//  autopilot
//
//  Created by Ludger Heide on 13.12.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#include "altitudePitchController.h"
#include "global.h"
#include "bmp.h"
#include "utils.h"
#include <math.h>

#define MIN_ALTITUDE_DIFFERENCE 100 //The altitude difference that is regared as zero in centimeters
#define MAX_ALTITUDE_DIFFERENCE 10000 //The altitude difference at which we command full climb/descend

#define MAX_RATE_OF_CLIMB 100 //Rate of climb in centimeters per second

#define PITCH_PROPORTIONAL_GAIN 1.0
#define PITCH_INTEGRAL_GAIN 1.0
#define MAX_CLIMB_ERROR 10000.0 //In centimeters

#define MAX_PITCH_ANGLE 20 //The maximum up/down pitch
#define MAX_PITCH_INTEGRAL 20


#define MAX_ROTATION M_PI //The rotation a full stick correspinds to (in rad/s)
#define ELEVATOR_PROPORTIONAL_GAIN 0.66
#define ELEVATOR_INTEGRAL_GAIN 0.33
#define ELEVATOR_DIFFERENTIAL_GAIN 0.5

s32 lastAltitude;
u32 lastAltitudeTime;
float integratedAltitude;

u32 lastPitchTime;
float integratedPitch;

//Calculates the rate of climb for a given target altitude
//INPUT: Altitude in centimeters
//OUTPUT: Value between -127 and 127 indicating climb(positive) or descend(negative)
s08 calculateRateOfClimb(s32 targetAltitude) {
    s32 currentAltitude = pressureToAltitude(curPressure.pressure, seaLevelPressure) * 100;
    
    s32 diff = targetAltitude - currentAltitude;
    
    if(ABS(diff) <= MIN_ALTITUDE_DIFFERENCE) {
        return 0;
    } else {
        return maps32(diff, -MAX_ALTITUDE_DIFFERENCE, MAX_ALTITUDE_DIFFERENCE, INT8_MIN, INT8_MAX);
    }
}

//Calculate the pitch angle for a given rate of climb
//Using a PI controller
//INPUT: signed 8-Bit value indicating the rate of climb
//OUTPUT: signed 8-Bit value indicating the desired pitch angle
s08 calculatePitchAngle(s08 targetRateOfClimb) {
    //Integral: Integrate the rate of climb error over time
    //If we have new data available, update the integrator
    if(curPressure.timestamp - lastAltitudeTime) {
        //First, calculate the current altitude in centimeters
        s32 currentAltitude = pressureToAltitude(curPressure.pressure, seaLevelPressure) * 100;
        
        //Next, subtract it from the last altitude and set the current altitude as last altitude
        s32 climbStepSize = currentAltitude - lastAltitude;
        lastAltitude = currentAltitude;
        
        //Now find the difference between actual climb and target rate of climb
        float targetClimb = maps16(targetRateOfClimb, INT8_MIN, INT8_MAX, -MAX_RATE_OF_CLIMB, MAX_RATE_OF_CLIMB) * ((curPressure.timestamp - lastAltitudeTime)/(float)1000000);
        float climbError = targetClimb - climbStepSize;
        
        //Add it to the integral
        integratedAltitude += climbError * ((curPressure.timestamp - lastAltitudeTime)/(float)1000000);
        
        //Anti-windup
        if(integratedAltitude > MAX_CLIMB_ERROR) {
            integratedAltitude = MAX_CLIMB_ERROR;
        } else if (integratedAltitude < -MAX_CLIMB_ERROR) {
            integratedAltitude = -MAX_CLIMB_ERROR;
        }
    }
    
    //Add the integral and proportional together to a 16 bit value and map it to the output
    s16 proportional_integral = targetRateOfClimb * (PITCH_PROPORTIONAL_GAIN * 64); //+-8192
    proportional_integral += integratedAltitude * (PITCH_INTEGRAL_GAIN * (8192/MAX_CLIMB_ERROR)); //+-8192 as well
    
    return maps16(proportional_integral, INT16_MIN/2, INT16_MAX/2, INT8_MIN, INT8_MAX);
}

//Calculate the rudder for a given pitch angle using a PID controller
//INPUT: signed 8-bit value indicating the desired pitch angle
//OUTPUR: *unsigned* 8-bit value to go to the servo
u08 calculateElevatorValue(s08 targetPitchAngle) {
    
    //Get the current pitch
    s08 currentPitch = currentAttitude.pitch; //Degrees (+-90)
    
    //Next, subtract it from the target pitch to get the eror
    s08 targetPitch = maps16(targetPitchAngle, INT8_MIN, INT8_MAX, -MAX_PITCH_ANGLE, MAX_PITCH_ANGLE);
    s16 pitchError = targetPitch - currentPitch;
    
    //Proportional
    float pitchProportionalNormalized =  mapfloat(pitchError, -MAX_PITCH_ANGLE, MAX_PITCH_ANGLE, -1, 1);
    
    //Add it to the integral
    integratedPitch += pitchError * ((currentAttitude.timestamp - lastPitchTime)/(float)1000);
    lastPitchTime = currentAttitude.timestamp;
    
    //Anti-windup
    if(integratedPitch > MAX_PITCH_INTEGRAL) {
        integratedPitch = MAX_PITCH_INTEGRAL;
    } else if (integratedPitch < -MAX_PITCH_INTEGRAL) {
        integratedPitch = -MAX_PITCH_INTEGRAL;
    }
    float pitchIntegralNormalized = mapfloat(integratedPitch, -MAX_PITCH_INTEGRAL, MAX_PITCH_INTEGRAL, -1, 1);
    
    //Differential: Take the current pitch rotation, compare it to the pitch error
    float wantedPitchRotation = mapfloat(targetPitchAngle, INT8_MIN, INT8_MAX, -MAX_ROTATION, MAX_ROTATION);
    float pitchDifference = -1 * (curGyro.x - wantedPitchRotation);
    float pitchDerivNormalized = mapfloat(pitchDifference, -MAX_ROTATION, MAX_ROTATION, -1, 1);
    
    //Add the three together and map them correctly
    float normalizedOutput = pitchProportionalNormalized * ELEVATOR_PROPORTIONAL_GAIN + pitchIntegralNormalized * ELEVATOR_INTEGRAL_GAIN + pitchDerivNormalized * ELEVATOR_DIFFERENTIAL_GAIN;
    return mapfloat(normalizedOutput, -1, 1, 0, UINT8_MAX);
}