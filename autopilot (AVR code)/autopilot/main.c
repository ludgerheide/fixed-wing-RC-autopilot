#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <assert.h>

#include "uart4.h"
#include "utils.h"
#include "imuBoard.h"
#include "timer.h"
#include "gps.h"

#include "pinSetup.h"

void initIO(void) {
    printfAttachToUart();
    timerInit();
//    IMUinit();
    gpsInit();
}

int main(void) {
    initIO();
    
    while (1) {
        if(gpsCheck()) {
            printf("new sentences @%lu\r\n", millis());
            gpsUpdate();
            printf("Position: %4.2f, %4.2f Altitude: %3.1f Time: %.2f Updates: %u\r\n", GpsInfo.PosLLA.lat, GpsInfo.PosLLA.lon, GpsInfo.PosLLA.alt, GpsInfo.PosLLA.TimeOfFix, GpsInfo.PosLLA.updates);
            printf("Sattelites: %u Heading: %4.2f Speed: %4.2f Time: %.2f Updates: %u\r\n", GpsInfo.numSVs, GpsInfo.VelHS.heading, GpsInfo.VelHS.speed, GpsInfo.VelHS.TimeOfFix, GpsInfo.VelHS.updates);
        }
    }
    return 0; // never reached
}