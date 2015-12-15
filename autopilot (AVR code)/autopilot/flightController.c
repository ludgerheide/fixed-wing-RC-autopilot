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

#include <math.h>

#define INPUT_TIMEOUT 500 //Maximum age of input command set in milliseconds
#define SENSOR_TIMEOUT 250 //Maximum age of sensor data to be considered valid
#define MAX_ROTATION (M_PI/4.0) //The rotation a full stick correspinds to (in rad/s)

#define THRESHOLD_VOLTAGE 7.0

static void degradedUpdate(void);
static void flyByWireUpdate(void);

void updateFlightControls(void) {
    //Check the age of our inputCommandSet. If it is too old, go to degraded mode
    //If it is good, put it out
    u32 commandSetAge = millis() - inputCommandSet.timestamp;
    if(commandSetAge > INPUT_TIMEOUT) {
        currentFlightMode = m_degraded;
        degradedUpdate();
    } else {
        currentFlightMode = m_flybywire;
        flyByWireUpdate();
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
    
    //Only allow 50% thrust if the voltage if below 7V
    if(curBattery.voltage < THRESHOLD_VOLTAGE) {
        outputCommandSet.thrust = MIN(inputCommandSet.thrust, 127);
    } else {
        outputCommandSet.thrust = inputCommandSet.thrust;
    }
    
    //Send the pitch stock to the pitch controller
    s08 pitchStickSigned = maps16(inputCommandSet.pitch, 0, UINT8_MAX, INT8_MIN, INT8_MAX);
    outputCommandSet.pitch = calculateElevatorValue(pitchStickSigned);
    
    //Interpret the stick values as rotation requests
    float wantedYawRotation = mapfloat(inputCommandSet.yaw, 0, 255, -MAX_ROTATION, MAX_ROTATION);
    float rotationDifference = curGyro.z - wantedYawRotation;
    outputCommandSet.yaw = mapfloat(rotationDifference, MAX_ROTATION, -MAX_ROTATION, 0, 255);
}