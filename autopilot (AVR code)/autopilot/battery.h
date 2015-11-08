//
//  battery.h
//  autopilot
//
//  Created by Ludger Heide on 07.11.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#ifndef battery_h
#define battery_h

#include "avrlibtypes.h"

//Structs
typedef struct {
    float voltage;
    float current;
    u32 timestamp;
} batteryEvent;

//Initializes the ADC. Setd everything up and starts the first volatage measurement
void batteryInit(void);

//fills a batterys event withg current and voltage
void batteryGetData(batteryEvent* event);

#endif /* battery_h */
