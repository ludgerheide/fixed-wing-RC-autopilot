//
//  battery.c
//  autopilot
//
//  Created by Ludger Heide on 07.11.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#include "battery.h"
#include "avrlibdefs.h"
#include "pinSetup.h"
#include "timer.h"
#include <avr/interrupt.h>
#include <avr/io.h>
#include <assert.h>
#include <stdio.h>

#ifndef CRITICAL_SECTION_START
#define CRITICAL_SECTION_START	unsigned char _sreg = SREG; cli()
#define CRITICAL_SECTION_END	SREG = _sreg
#endif

// A2D clock prescaler select
//		*selects how much the CPU clock frequency is divided
//		to create the A2D clock frequency
//		*lower division ratios make conversion go faster
//		*higher division ratios make conversions more accurate
#define ADC_PRESCALE_DIV2		0x00	///< 0x01,0x00 -> CPU clk/2
#define ADC_PRESCALE_DIV4		0x02	///< 0x02 -> CPU clk/4
#define ADC_PRESCALE_DIV8		0x03	///< 0x03 -> CPU clk/8
#define ADC_PRESCALE_DIV16		0x04	///< 0x04 -> CPU clk/16
#define ADC_PRESCALE_DIV32		0x05	///< 0x05 -> CPU clk/32
#define ADC_PRESCALE_DIV64		0x06	///< 0x06 -> CPU clk/64
#define ADC_PRESCALE_DIV128		0x07	///< 0x07 -> CPU clk/128
// do not change the mask value
#define ADC_PRESCALE_MASK		0x07

// A2D voltage reference select
//		*this determines what is used as the
//		full-scale voltage point for A2D conversions
#define ADC_REFERENCE_AREF		0x00	///< 0x00 -> AREF pin, internal VREF turned off
#define ADC_REFERENCE_AVCC		0x01	///< 0x01 -> AVCC pin, internal VREF turned off
#define ADC_REFERENCE_RSVD		0x02	///< 0x02 -> Reserved
#define ADC_REFERENCE_256V		0x03	///< 0x03 -> Internal 2.56V VREF
// default value
#define ADC_REFERENCE			ADC_REFERENCE_AVCC
// do not change the mask value
#define ADC_REFERENCE_MASK		0xC0

// bit mask for A2D channel multiplexer
#define ADC_MUX_MASK			0x1F

// channel defines (for reference and use in code)
// these channels supported by all AVRs with A2D
#define ADC_CH_ADC0				0x00
#define ADC_CH_ADC1				0x01
#define ADC_CH_ADC2				0x02
#define ADC_CH_ADC3				0x03
#define ADC_CH_ADC4				0x04
#define ADC_CH_ADC5				0x05
#define ADC_CH_ADC6				0x06
#define ADC_CH_ADC7				0x07

//The possible modes of the ADC
typedef enum {
    ADC_VOLTAGE_FIRST,
    ADC_VOLTAGE_SECOND,
    ADC_CURRENT_FIRST,
    ADC_CURRENT_SECOND
} adcMode;

static volatile adcMode theMode;

//Value between 0 and 1023. 512 means 0 amps, 1023 means 25A, 0 -25A
static volatile short batteryCurrent;

//Value between 0 and 1023. 0 means 0 V vBat, 1023 means 1.1 V measured, that corresponds to 8,433333333333333 V vBat
static volatile short batteryVoltage;

// configure A2D converter clock division (prescaling)
static void a2dSetPrescaler(unsigned char prescale)
{
    outb(ADCSRA, ((inb(ADCSRA) & ~ADC_PRESCALE_MASK) | prescale));
}

// configure A2D converter voltage reference
static void a2dSetReference(unsigned char ref)
{
    outb(ADMUX, ((inb(ADMUX) & ~ADC_REFERENCE_MASK) | (ref<<6)));
}

// start a conversion on the selected channel
static void a2dStartConversion(u08 channel) {
    assert(channel < 16);
    
    if(channel < 8) {
        cbi(ADCSRB, MUX5);
        outb(ADMUX, (inb(ADMUX) & ~ADC_MUX_MASK) | (channel & ADC_MUX_MASK));	// set channel
    } else {
        sbi(ADCSRB, MUX5);
        outb(ADMUX, (inb(ADMUX) & ~ADC_MUX_MASK) | ((channel-8) & ADC_MUX_MASK));	// set channel
    }
    
    sbi(ADCSRA, ADIF);						// clear hardware "conversion complete" flag
    sbi(ADCSRA, ADSC);						// start conversion
}

//Initializes the ADC. Set everything up and starts the first volatage measurement
void batteryInit(void) {
    CRITICAL_SECTION_START;
    sbi(ADCSRA, ADEN);				      // enable ADC (turn on ADC power)
    a2dSetPrescaler(ADC_PRESCALE_DIV128); // set default prescaler
    cbi(ADMUX, ADLAR);			          // set to right-adjusted result
    sbi(ADCSRA, ADIE);				      // enable ADC interrupts
    
    //Set up for a first voltage measurement
    theMode = ADC_VOLTAGE_FIRST;
    a2dSetReference(ADC_REFERENCE_AVCC);
    a2dStartConversion(VOLTAGE_CHANNEL);
    
    CRITICAL_SECTION_END;
}

//Converts the raw voltage to a real value
static float convertVoltage(void) {
    CRITICAL_SECTION_START;
    float voltage = batteryVoltage;
    CRITICAL_SECTION_END;
    
    //For 1.1V reference
    //const float conversionFactor = 8.43333333 / 1023;
    //For 2.56V reference
    //const float conversionFactor = 19.626666666 / 1023;
    //For 5V reference
    const float conversionFactor = 38.33333333 / 1023;
    
    return voltage * conversionFactor;
}

//Converts the raw current to a real value
static float convertCurrent(void) {
    CRITICAL_SECTION_START;
    float current = batteryCurrent;
    CRITICAL_SECTION_END;
    //Subtract to get to the ± 511.5 range
    current -= 511.5;
    
    const float conversionFactor = 50.0 / 1023;
    
    return current * conversionFactor;
}

//fills a batterys event withg current and voltage
void batteryGetData(batteryEvent* event) {
    event->voltage = convertVoltage();
    event->current = convertCurrent();
    event->timestamp = micros();
}

//ISR for the ADC interrupt. Save (if applicable), advance the mode and start a new measurement
ISR(ADC_vect) {
    switch (theMode) {
        case ADC_VOLTAGE_FIRST:
            //Ignore the first reuslt and measure again (so the reference can stabilize)
            a2dStartConversion(VOLTAGE_CHANNEL);
            
            theMode = ADC_VOLTAGE_SECOND;
            break;
            
        case ADC_VOLTAGE_SECOND:
            //Read the result
            batteryVoltage = inb(ADCL) | (inb(ADCH)<<8);
            
            a2dStartConversion(CURRENT_CHANNEL);
                              
            theMode = ADC_CURRENT_FIRST;
            break;
            
        case ADC_CURRENT_FIRST:
            //Ignore the result and measure again
            a2dStartConversion(CURRENT_CHANNEL);
            
            theMode = ADC_CURRENT_SECOND;
            break;
            
        case ADC_CURRENT_SECOND:
            //Read the result
            batteryCurrent = inb(ADCL) | (inb(ADCH)<<8);
            
            a2dStartConversion(VOLTAGE_CHANNEL);
            theMode = ADC_VOLTAGE_FIRST;
            break;
            
        default:
            break;
    }
}