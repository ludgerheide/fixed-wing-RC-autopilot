//
//  communicationsHandler.h
//  autopilot
//
//  Created by Ludger Heide on 06.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#ifndef communicationsHandler_h
#define communicationsHandler_h

#include <stdio.h>
#include "avrlibtypes.h"

#include "pb_decode.h"
#include "pb_encode.h"
#include "pb_common.h"

#include "xBee.h"
#include "communicationProtocol.pb.h"

//Initializes the communications (the xBee serial and the RTS output pin)
void commsInit(void);

void commsProcessMessage(char* message, u08 size);

//Function to convert degrees to a fixed-point 16 byte integer
s16 degreesToInt(float degrees);

//Converts a fixed-point interger to a float
float intToDegrees(s16 fixedPoint);

//Telemetry goes out over the radio and is a small packet with posoition, velocity and attitude
void commsCheckAndSendTelemetry(void);

//Logging goes out over the serial port and contains *a lot* of data
void commsCheckAndSendLogging(void);

//Called when a status is received
void txStatusHandler(uint8_t frameID, uint8_t retryCount, uint8_t txStatus);


#endif /* communicationsHandler_h */
