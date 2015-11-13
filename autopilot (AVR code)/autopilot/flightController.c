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
#include <math.h>

#define INPUT_TIMEOUT 500 //Maximum age of input command set in milliseconds
#define SENSOR_TIMEOUT 250 //Maximum age of sensor data to be considered valid
#define MAX_ROTATION (M_PI/4.0)

static void degradedUpdate(void);
static void passthroughUpdate(void);
static void flyByWireUpdate(void);

void updateFlightControls(void) {
    //Check the age of our inputCommandSet. If it is too old, go to degraded mode
    //If it is good, put it out
    u32 commandSetAge = millis() - inputCommandSet.timestamp;
    if(commandSetAge > INPUT_TIMEOUT) {
        currentFlightMode = m_degraded;
        degradedUpdate();
    } else {
        currentFlightMode = m_passThrough;
        passthroughUpdate();
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

static void passthroughUpdate(void) {
    // Set servos
    if((inputCommandSet.timestamp) - (outputCommandSet.timestamp)) {
        outputCommandSet.timestamp = millis();
        outputCommandSet.yaw = inputCommandSet.yaw;
        outputCommandSet.pitch = inputCommandSet.pitch;
        outputCommandSet.thrust = inputCommandSet.thrust;
    }
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
    float out = (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    if(out < out_min) {
        out = out_min;
    } else if(out > out_max) {
        out = out_max;
    }
    return out;
}