//
//  gps.h
//  avr - flyByWire
//
//  Created by Ludger Heide on 01.10.15.
//  Copyright © 2015 LH Technologies. All rights reserved.
//

#ifndef gps_h
#define gps_h

#include "nmea.h"

#ifdef GPS_DEBUG
#include <stdio.h>
#endif

extern GpsInfoType GpsInfo;

//Sets up the GPS Serial port, sends the initialization commands to the GPS and switches the baud rate
void gpsInit(void);

//Checks if the GPS_UART buffer is ready for an NMEA update
BOOL gpsCheck(void);

//Calls the NMEA parser and updates the struct
void gpsUpdate(void);

//This gets attached to the UART RX handler.
// It counts the '\r\n' combinations that mark the end of the packets.
// When two packets (GGA + VTG) Are in the buffer, it sets a flag so that they are parsed in the next run
void gpsRxHandler(unsigned char c);

#endif /* gps_h */