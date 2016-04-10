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

const float altitudeFilterCoefficient = 0.25; //Impact of the new value, for example .25 means 1/4 of the new value and 3/4 of the old

const float maxAltitudeIntegral = 10.0 * 100.0 * 10.0; //The maximum value of the integral in centimeters * seconds
const s32 maxAltitudeProportional = 25 * 100; //The maximum Proportinal error in centimeters

//Do not change the "8192", but the factor after it
const s16 maxNormalizedIntegral = 8192 * 1.0;
const s16 maxNormalizedProportional = 8192 * 1.0;

u32 lastAltitudeTime;
float smoothedAltitude;
float integratedAltitudeError;

//Calculate the pitch angle for a given target altitude
//Using a PI controller
//INPUT: signed 32-Bit value indicating the target altitude
//OUTPUT: signed 8-Bit value indicating the desired pitch angle
s08 calculatePitchAngle(s32 targetAltitude) {
    //Check if we have a new altitude available and update our smoothed altitude
    s32 altitudeError;
    u32 altitudeAge = curPressure.timestamp - lastAltitudeTime;
    if(altitudeAge > 0) {
        float currentAltitude = pressureToAltitude(curPressure.pressure, seaLevelPressure);
        smoothedAltitude = altitudeFilterCoefficient * currentAltitude + (1 - altitudeFilterCoefficient) * smoothedAltitude;
        
        //Update the time and the integral
        altitudeError = targetAltitude - (smoothedAltitude * 100);
        integratedAltitudeError += altitudeError * (altitudeAge / (float)1000000);
        
        lastAltitudeTime = curPressure.timestamp;
        
        //Anti-windup
        if(integratedAltitudeError > maxAltitudeIntegral) {
            integratedAltitudeError = maxAltitudeIntegral;
        } else if(integratedAltitudeError < -maxAltitudeIntegral) {
            integratedAltitudeError = -maxAltitudeIntegral;
        }
    } else {
        altitudeError = targetAltitude - (smoothedAltitude * 100);
    }
    
    s16 normalized_integral = mapfloat(integratedAltitudeError, -maxAltitudeIntegral, maxAltitudeIntegral, -maxNormalizedIntegral, maxNormalizedIntegral);
    s16 normalized_proportional = maps32(altitudeError, -maxAltitudeProportional, maxAltitudeProportional, -maxNormalizedProportional, maxNormalizedProportional);
    
    //Add the integral and proportional together to a 16 bit value and map it to the output
    s16 proportional_integral = normalized_integral + normalized_proportional;
    
    return maps16(proportional_integral, INT16_MIN/2, INT16_MAX/2, INT8_MIN, INT8_MAX);
}

#define MAX_PITCH_ANGLE 20 //The maximum up/down pitch
#define MAX_PITCH_INTEGRAL 20

#define MAX_ROTATION M_PI //The rotation a full stick correspinds to (in rad/s)
#define ELEVATOR_PROPORTIONAL_GAIN 0.66
#define ELEVATOR_INTEGRAL_GAIN 0.33
#define ELEVATOR_DIFFERENTIAL_GAIN 0.5

u32 lastPitchTime;
float integratedPitch;

//Calculate the elevator for a given pitch angle using a PID controller
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