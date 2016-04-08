//
//  flightControllerTypes.h
//  autopilot
//
//  Created by Ludger Heide on 08.04.16.
//  Copyright © 2016 Ludger Heide. All rights reserved.
//

#ifndef flightControllerTypes_h
#define flightControllerTypes_h

#include "avrlibtypes.h"

typedef struct {
    u32 timestamp;
    
    u08 yaw;
    u08 pitch;
    u08 thrust;
} commandSet_struct;
commandSet_struct inputCommandSet;
commandSet_struct outputCommandSet;

typedef struct {
    u32 timestamp;
    
    BOOL altitudeInUse;
    s32 altitude; //Centimeters
    s08 pitchAngle; //Relative value
    
    BOOL headingInUse;
    u16 heading; //Degrees
    s08 rateOfTurn; //Relative value
} autonomousUpdateType;
autonomousUpdateType autonomousUpdate;

typedef enum {
    m_degraded = 0,
    m_passThrough = 1,
    m_flybywire = 2,
    m_autonomous = 3,
    m_autonomousReturnToBase = 4
} flightMode;
flightMode currentFlightMode;

typedef struct {
    u32 timestamp;
    
    float latitude;
    float longitude;
    float altitude; //Meters ASL
} waypoint;
waypoint homeBase;

#endif /* flightControllerTypes_h */
