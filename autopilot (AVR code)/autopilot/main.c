#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <assert.h>

#include "gyro.h"
#include "accelMag.h"
#include "bmp.h"

#include "utils.h"
#include "imuBoard.h"
#include "timer.h"
#include "gps.h"
#include "pinSetup.h"
#include "MadgwickAHRS.h"
#include "communicationsHandler.h"

#include "uart4.h"

void initIO(void) {
    printfAttachToUart();
    timerInit();
    IMUinit();
    gpsInit();
    commsInit();
}

int main(void) {
    initIO();
    
    while (1) {
        
    }
    return 0; // never reached
}