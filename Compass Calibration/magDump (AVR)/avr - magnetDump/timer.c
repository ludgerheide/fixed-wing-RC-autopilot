//
//  timer.c
//  avr - flyByWire
//
//  Created by Ludger Heide on 25.09.15.
//  Copyright © 2015 LH Technologies. All rights reserved.
//

#include "timer.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include "avrlibdefs.h"

#include <stdio.h>

#ifndef CRITICAL_SECTION_START
#define CRITICAL_SECTION_START	unsigned char _sreg = SREG; cli()
#define CRITICAL_SECTION_END	SREG = _sreg
#endif

#define MICROSECONDS_PER_TICK ((64*1000000UL)/F_CPU)

static volatile u32 millisCounter;

//The interrupt callback function
static void (*timerInterruptCallback)(u32 millis);

void timerInit(void) {
    //Clear the interrupt callback
    timerInterruptCallback = 0;
    
    //Set the CTC match to 250 timer ticks (1000ms @ 16MHz)
    OCR0A = 249;
    
    //Initialize the timer in CTC mode
    sbi(TCCR0A, WGM01);
    
    //Enable the "compare match a" interrupt
    sbi(TIMSK0, OCIE0A);
    
    //Set the prescaler for timer 0 to 64 (this starts the timer)
    outb(TCCR0B, BV(CS00)|BV(CS01));
}

u32 millis(void) {
    CRITICAL_SECTION_START;
    u32 myMillis = millisCounter;
    CRITICAL_SECTION_END;
    return myMillis;
}

// Interrupt-compatible version of micros
// Theory: Take the timer register and check if the timer interrupt is pending.
//If yes, add 1 to the milliseconds
u32 micros(void) {
    CRITICAL_SECTION_START;
    u32 microseconds = TCNT0;
    if(microseconds != 249 && inb(TIFR0) & _BV(OCF0A)) {
        //We have a pending millis increment
        microseconds += (250 * millisCounter);
        microseconds += 250;
        CRITICAL_SECTION_END;
        return microseconds * MICROSECONDS_PER_TICK;
    }
    microseconds += (250 * millisCounter);
    CRITICAL_SECTION_END;
    return microseconds * MICROSECONDS_PER_TICK;
}

//The timer 0 overflow ISR
ISR(TIMER0_COMPA_vect) {
    millisCounter++;
    if(timerInterruptCallback) {
        timerInterruptCallback(millisCounter);
    }
}

void timerSetInterruptCallback(void (*timerInterrupt_func)(u32 millis)) {
    timerInterruptCallback = timerInterrupt_func;
}