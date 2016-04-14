//
//  raspiComms.h
//  autopilot
//
//  Created by Ludger Heide on 10.04.16.
//  Copyright © 2016 Ludger Heide. All rights reserved.
//

#ifndef raspiComms_h
#define raspiComms_h

#include "avrlibtypes.h"

extern volatile BOOL raspiNewMessageReady;

//Initializes comms with the RasPi
void raspiInit(void);

//Called each time the raspi has sent a byte. ISR method – needs to be fast
void raspiByteReceiver(u08 c);

//Called when a complete message os in the buffer
void raspiHandleMessage(void);

#endif /* raspiComms_h */
