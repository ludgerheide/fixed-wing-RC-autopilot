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

#include <pb_decode.h>
#include <pb_encode.h>
#include <pb_common.h>

#include "xBee.h"
#include "communicationProtocol.pb.h"

//Initializes the communications (the xBee serial and the RTS output pin)
void commsInit(void);

void commsProcessMessage(char* message, u08 size);

void commsSendStatusReport(void);

#endif /* communicationsHandler_h */
