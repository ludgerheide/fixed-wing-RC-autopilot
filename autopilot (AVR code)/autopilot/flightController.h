//
//  flightController.h
//  autopilot
//
//  Created by Ludger Heide on 13.11.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#ifndef flightController_h
#define flightController_h

#include <stdio.h>

void updateFlightControls(void);
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);

#endif /* flightController_h */
