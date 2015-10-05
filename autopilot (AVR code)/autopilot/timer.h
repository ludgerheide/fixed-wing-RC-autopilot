//
//  timer.h
//  avr - flyByWire
//
//  Created by Ludger Heide on 25.09.15.
//  Copyright © 2015 LH Technologies. All rights reserved.
//

#ifndef timer_h
#define timer_h

#include "avrlibtypes.h"

//Initializes timer 0 for micro/millisecond counting
void timerInit(void);

//Returns the millisecond counter's value (resets every 49 days)
u32 millis(void);

//Returns the microseconds since the start (4µs accuracy @ 16MHz)
u32 micros(void);

//Attaches a function that is called each timer interrupt (1ms @16MHz)
void timerSetInterruptCallback(void (*timerInterrupt_func)(u32 millis));

#endif /* timer_h */
