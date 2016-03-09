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
#include "Flugregler.h"

#include <math.h>

#define INPUT_TIMEOUT 250 //Maximum age of input command set in milliseconds
#define SENSOR_TIMEOUT 100 //Maximum age of sensor data to be considered valid
#define MAX_ROTATION (M_PI/4.0) //The rotation a full stick correspinds to (in rad/s)

#define THRESHOLD_VOLTAGE 6.75

//Static variables
static u08 maxThrust = 255;

static u32 lastFlyByWireUpdate = 0;
static const u32 flyByWireupdateInterval = 20; //milliseconds between fly by wire updates

//Static method declarations
static void degradedUpdate(void);
static void passthroughUpdate(void);
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

void flightControllerInit(void) {
    Flugregler_initialize();
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
    outputCommandSet.timestamp = millis();
    
    //Decrease thrust limit if battery voltage is below minimum, increase otherwise
    if(curBattery.voltage < THRESHOLD_VOLTAGE) {
        maxThrust--;
    } else if (maxThrust < UINT8_MAX) {
        maxThrust++;
    }
    outputCommandSet.thrust = MIN(inputCommandSet.thrust, maxThrust);
    
    //Send the pitch stock to the pitch controller
    outputCommandSet.pitch = inputCommandSet.pitch;
    
    //Interpret the stick values as rotation requests (negate because stick is wrong way
    outputCommandSet.yaw = inputCommandSet.yaw;
}

static void flyByWireUpdate() {
    u32 currentTime = millis();
    if(currentTime - lastFlyByWireUpdate >= flyByWireupdateInterval) {
        //Update the time now because we are using the VALUES of now for the update, not of after it's done
        lastFlyByWireUpdate = currentTime;
        
        //Setup the inputs of the flight controller
        Flugregler_U.currentAttitude.courseMagnetic = currentAttitude.courseMagnetic;
        Flugregler_U.currentAttitude.pitch = currentAttitude.pitch;
        Flugregler_U.currentAttitude.roll = currentAttitude.roll;
        
        Flugregler_U.barometricAltitude_m.altitude = pressureToAltitude(curPressure.pressure, seaLevelPressure) * 100;
        
        Flugregler_U.targetHeading_g.justRateOfTurnEnabled = true;
        Flugregler_U.targetHeading_g.targetRateOfTurn = (s16) inputCommandSet.yaw - 128;
        Flugregler_U.targetHeading_g.targetHeading = 0;
        
        Flugregler_U.targetPitch_f.justPitchEnabled = true;
        Flugregler_U.targetPitch_f.targetPitch = maps16(inputCommandSet.pitch, 0, UINT8_MAX, -20, 20);
        Flugregler_U.targetPitch_f.targetAltitude = 10000;
        
        Flugregler_U.gyro.x = curGyro.x;
        Flugregler_U.gyro.y = curGyro.y;
        Flugregler_U.gyro.z = curGyro.z;
        
        Flugregler_step();
        
        //Put the outputs of the flight controller into the output command set
        outputCommandSet.timestamp = millis();
        outputCommandSet.yaw = Flugregler_Y.outputCommandSet.rudder;
        outputCommandSet.pitch = Flugregler_Y.outputCommandSet.elevator;
        
        //Decrease thrust limit if battery voltage is below minimum, increase otherwise
        if(curBattery.voltage < THRESHOLD_VOLTAGE) {
            maxThrust--;
        } else if (maxThrust < UINT8_MAX) {
            maxThrust++;
        }
        outputCommandSet.thrust = MIN(inputCommandSet.thrust, maxThrust);
    }
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