//
//  flightController.c
//  autopilot
//
//  Created by Ludger Heide on 13.11.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#include "flightController.h"
#include "timer.h"
#include "global.h"
#include "utils.h"
#include "altitudePitchController.h"
#include "yawController.h"

#include <math.h>

#define INPUT_TIMEOUT 250 //Maximum age of input command set in milliseconds
#define SENSOR_TIMEOUT 100 //Maximum age of sensor data to be considered valid
#define MAX_ROTATION (M_PI/4.0) //The rotation a full stick correspinds to (in rad/s)

#define THRESHOLD_VOLTAGE 6.75

//Static variables
static u08 maxThrust = 255;

//Static method declarations
static void degradedUpdate(void);
static void flyByWireUpdate(void);
static BOOL flyByWireSensorsGood(void);
static BOOL commandSetGood(void);

void checkSensorsAndSetFlightMode(void) {
    if(flyByWireSensorsGood() && commandSetGood()) {
        currentFlightMode = m_flybywire;
    } else {
        currentFlightMode = m_degraded;
    }
}

void updateFlightControls(void) {
    if(currentFlightMode == m_flybywire) {
        flyByWireUpdate();
    } else {
        currentFlightMode = m_degraded;
        degradedUpdate();
    }
}

static void degradedUpdate(void) {
    u32 now = millis();
    
    //Turn off the engine, try to maintain zero pitch with an easy algorithm and try to maintain zero yawing rate
    
    //First, the engine
    outputCommandSet.timestamp = now;
    outputCommandSet.thrust = 0;
    
    //Now, pitch
    if(now - currentAttitude.timestamp > SENSOR_TIMEOUT) {
        outputCommandSet.pitch = 127; //Neutral pitch
    } else {
        float pitch = currentAttitude.pitch; //This is a +-90 range value
        outputCommandSet.pitch = mapfloat(pitch, 20, -20, 0, 255); //We give full rudder at +-45
    }
    
    //Finally, yaw
    if(now - (curGyro.timestamp / 1000) > SENSOR_TIMEOUT) { //gyro timestamps are in microseconds
        outputCommandSet.yaw = 127;
    } else {
        float yawRate = curGyro.z; //In radians per second
        outputCommandSet.yaw = mapfloat(yawRate, +MAX_ROTATION, -MAX_ROTATION, 0, 255);
    }
}


static void flyByWireUpdate(void) {
    outputCommandSet.timestamp = millis();
    
    //Decrease thrust limit if battery voltage is below minimum, increase otherwise
    if(curBattery.voltage < THRESHOLD_VOLTAGE) {
        maxThrust--;
    } else if (maxThrust < UINT8_MAX) {
        maxThrust++;
    }
    outputCommandSet.thrust = MIN(inputCommandSet.thrust, maxThrust);
    
    //Send the pitch stock to the pitch controller
    s08 pitchStickSigned = mapfloat(inputCommandSet.pitch, 0, UINT8_MAX, INT8_MIN, INT8_MAX);
    outputCommandSet.pitch = calculateElevatorValue(pitchStickSigned);
    
    //Interpret the stick values as rotation requests (negate because stick is wrong way
    s08 yawStickSigned = mapfloat(inputCommandSet.yaw, UINT8_MAX, 0, INT8_MIN, INT8_MAX);
    outputCommandSet.yaw = calculateRudderValue(yawStickSigned);
}

static BOOL flyByWireSensorsGood(void) {
    u32 now = micros();
    
    //Check timing of accelerometer and gyro
    if(now - curGyro.timestamp > (u32)1000 * SENSOR_TIMEOUT) {
        return FALSE;
    }
    
    if(now - curAccel.timestamp > (u32)1000 * SENSOR_TIMEOUT) {
        return FALSE;
    }
    
    if(millis() - currentAttitude.timestamp > SENSOR_TIMEOUT) {
        return FALSE;
    }
    
    return TRUE;
}

static BOOL commandSetGood(void) {
    u32 now = millis();
    
    //Chek timing of input command set
    if(now - inputCommandSet.timestamp > INPUT_TIMEOUT) {
        return FALSE;
    }
    return TRUE;
}