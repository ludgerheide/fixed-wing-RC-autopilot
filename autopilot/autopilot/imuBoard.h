//
//  imuBoard.h
//  avr - flyByWire
//
//  Created by Ludger Heide on 25.09.15.
//  Copyright © 2015 LH Technologies. All rights reserved.
//

#ifndef imuBoard_h
#define imuBoard_h

#include "avrlibtypes.h"

//This bit field defines which sensor has events queued
typedef struct {
    BOOL gyroEnabled;
    BOOL accelEnabled;
    BOOL magEnabled;
    BOOL bmpEnabled;
    
    BOOL gyroReady;
    BOOL accelReady;
    BOOL magReady;
    BOOL bmpReady;
} imuFlags;

extern volatile imuFlags theFlags;

//Initialize all sensors on the IMU board
void IMUinit(void);

//Gets called when an I2C tarnsmission is over
void customStopHandler(u08 statusReg, u08 deviceaddress);

//Called from timer.h when a millisecond has passes (to work with the time-bases controls of the BMP180)
void imuTimerTick(u32 millis);

#endif /* imuBoard_h */
