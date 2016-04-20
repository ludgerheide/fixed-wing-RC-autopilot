//
//  flightController.c
//  autopilot
//
//  Created by Ludger Heide on 13.11.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#include "avrlibtypes.h"

#include "flightControllerTypes.h"
#include "flightController.h"
#include "timer.h"
#include "global.h"
#include "utils.h"
#include "altitudePitchController.h"
#include "yawController.h"

#include "nmea.h"
#include "bmp.h"

#include <math.h>

#define INPUT_TIMEOUT 250 //Maximum age of input command set in milliseconds
#define AUTONOMOUS_INPUT_TIMEOUT 1100 //Maximum age of the autonoous update to be considered valis. Milliseconds
#define SENSOR_TIMEOUT 100 //Maximum age of sensor data to be considered valid. In milliseconds
#define GPS_TIMEOUT 1100 //Maximum age of a GPS dataset. In milliseconds
#define MAX_ROTATION (M_PI/4.0) //The rotation a full stick correspinds to (in rad/s)

#define THRESHOLD_VOLTAGE 6.75

//Static variables
static u08 maxThrust = 255;

//Static method declarations
static void degradedUpdate(void);
static void flyByWireUpdate(void);
static void autonomousControlUpdate(void);
static void autonomousReturnToBaseUpdate(void);
static BOOL flyByWireSensorsGood(void);
static BOOL autonomousSensorsGood(void);
static BOOL commandSetGood(void);
static BOOL autonomousUpdateGood(void);

void flightControllerInit(void) {
    //Initialize the input and output command set
    outputCommandSet.timestamp = 0;
    outputCommandSet.yaw = 127;
    outputCommandSet.pitch = 127;
    outputCommandSet.thrust = 0;
    
    inputCommandSet.timestamp = 0;
    inputCommandSet.yaw = 127;
    inputCommandSet.pitch = 127;
    inputCommandSet.thrust = 0;
    
    //Initialize the flight mode
    currentFlightMode = m_degraded;
    
    //Initiate the autonomous update to sane values
    autonomousUpdate.altitudeInUse = FALSE;
    autonomousUpdate.pitchAngle = 0;
    
    autonomousUpdate.headingInUse = FALSE;
    autonomousUpdate.rateOfTurn = 0;
}

flightMode checkSensorsAndSetFlightMode(void) {
    //Precedence of FLightmodes:
    //1. Fly by wire – if sensors permit
    //2. Autonomous path following – if sensors permit
    //3. Autonomous return to Base – if sensors permit
    //4. Degraded
    
    if(commandSetGood() && flyByWireSensorsGood()) {
        return m_flybywire;
    } else if(autonomousUpdateGood() && autonomousSensorsGood()) {
        return m_autonomous;
    } else if (autonomousSensorsGood()) {
        return m_degraded; //Since autonomousRTB is not yet validated, go to degraded
        //return m_autonomousReturnToBase;
    } else {
        return m_degraded;
    }
}

void updateFlightControls(void) {
    switch (currentFlightMode) {
        case m_degraded:
            degradedUpdate();
            break;
        
        case m_flybywire:
            flyByWireUpdate();
            break;
            
        case m_autonomous:
            autonomousControlUpdate();
            break;
            
        case m_autonomousReturnToBase:
            autonomousReturnToBaseUpdate();
            break;
            
        default:
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
    s08 pitchStickSigned = (s16)inputCommandSet.pitch + INT8_MIN;
    outputCommandSet.pitch = calculateElevatorValue(pitchStickSigned);
    
    //Interpret the stick values as rotation requests
    s08 yawStickSigned = (s16)inputCommandSet.yaw + INT8_MIN;
    outputCommandSet.yaw = calculateRudderValue(yawStickSigned);
}


static void autonomousControlUpdate(void) {
    outputCommandSet.timestamp = millis();
    
    //First, altitude/pitch
    //Check if we are maintaining pitch angle or altitude
    s08 pitchAngle;
    if(autonomousUpdate.altitudeInUse) {
        //Turn altitude into pitch angle
        pitchAngle = calculatePitchAngle(autonomousUpdate.altitude);
    } else {
        pitchAngle = autonomousUpdate.pitchAngle;
    }
    outputCommandSet.pitch = calculateElevatorValue(pitchAngle);
    
    //Calculate thrust according to 75% of max ± 25% for climb descent
    outputCommandSet.thrust = 191 + pitchAngle/2;
    
    //Last, yaw
    s08 targetRateOfTurn;
    if(autonomousUpdate.headingInUse) {
        targetRateOfTurn = calculateRateOfTurn(autonomousUpdate.heading);
    } else {
        targetRateOfTurn = autonomousUpdate.rateOfTurn;
    }
    
    outputCommandSet.yaw = calculateRudderValue(targetRateOfTurn);
}

static void autonomousReturnToBaseUpdate(void) {
    //Set autonomous update to "now"
    autonomousUpdate.timestamp = millis();
    
    //Calculate course to home base
    autonomousUpdate.headingInUse = TRUE;
#ifdef FLIGHT_CONTROLLER_DEBUG
    printf("Home base: lat %f, lon %f, alt %f\r\n", homeBase.latitude, homeBase.longitude, homeBase.altitude);
#endif
    
    autonomousUpdate.heading = bearingToCoordinates(GpsInfo.PosLLA.lat, GpsInfo.PosLLA.lon, homeBase.latitude, homeBase.longitude);
    
    //Update altitude to home base alt
    autonomousUpdate.altitudeInUse = TRUE;
    autonomousUpdate.altitude = homeBase.altitude * 100;
    
    //Now, do a "regular" autonomous control update
    autonomousControlUpdate();
}

static BOOL flyByWireSensorsGood(void) {
    u32 now = micros();
    
    //Check timing of accelerometer and gyro
    if(curGyro.timestamp == 0 || now - curGyro.timestamp > (u32)1000 * SENSOR_TIMEOUT) {
        return FALSE;
    }
    
    if(curAccel.timestamp == 0 || now - curAccel.timestamp > (u32)1000 * SENSOR_TIMEOUT) {
        return FALSE;
    }
    
    if(currentAttitude.timestamp == 0 || millis() - currentAttitude.timestamp > SENSOR_TIMEOUT) {
        return FALSE;
    }
    
    return TRUE;
}

static BOOL autonomousSensorsGood(void) {
    //First, check if the attitude (i.e flybywire) sensors are good
    if(flyByWireSensorsGood() == FALSE) {
        return FALSE;
    }
    
    u32 nowMillis = millis();
    u32 nowMicros = micros();
    
    //Then the compass
    if(curMag.timestamp == 0 || nowMicros - curMag.timestamp > (u32)1000 * SENSOR_TIMEOUT) {
        return FALSE;
    }
    
    //Then check the GPS
    if(GpsInfo.PosLLA.timestamp == 0 || nowMillis - GpsInfo.PosLLA.timestamp > GPS_TIMEOUT) {
        return FALSE;
    }
    
    //Check the altimeter
    if(curPressure.timestamp == 0 || nowMicros - curPressure.timestamp > (u32)1000 * SENSOR_TIMEOUT) {
        return FALSE;
    }
    
    return TRUE;
}

static BOOL commandSetGood(void) {
    u32 now = millis();
    
    //Chek timing of input command set
    if(inputCommandSet.timestamp == 0 || now - inputCommandSet.timestamp > INPUT_TIMEOUT) {
        return FALSE;
    }
    return TRUE;
}


static BOOL autonomousUpdateGood(void) {
    u32 now = millis();
    
    //Check age of autonomousUpdate
    if(autonomousUpdate.timestamp == 0 || now - autonomousUpdate.timestamp > AUTONOMOUS_INPUT_TIMEOUT) {
        return FALSE;
    }
    return TRUE;
}