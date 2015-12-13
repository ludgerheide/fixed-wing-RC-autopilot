#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include <assert.h>

#include "uart4.h"
#include "utils.h"
#include "imuBoard.h"

#include "pinSetup.h"

#include "bmp.h"
#include "gyro.h"
#include "accelMag.h"
#include "i2c.h"
#include "timer.h"

#ifndef CRITICAL_SECTION_START
#define CRITICAL_SECTION_START	unsigned char _sreg = SREG; cli()
#define CRITICAL_SECTION_END	SREG = _sreg
#endif

void initIO(void) {
    printfAttachToUart();
    timerInit();
    IMUinit();
}

int main(void) {
    initIO();
    
    magEvent myMag, myCompensatedMag;
    myMag.timestamp = 0;
    
    u32 diff;
    
    printf("Timestamp, x, y, z\r\n");
    while (1) {
        {
            CRITICAL_SECTION_START;
            diff = myMagRawData.timestamp - myMag.timestamp;
            CRITICAL_SECTION_END;
        }
        
        if(diff > 0) {
            //There is a new mag event, get it
            magGetData(&myMag);
            printf("mag-raw, %f, %f, %f\r\n", myMag.x, myMag.y, myMag.z);
            
            magCompensate(&myMag, &myCompensatedMag);
            printf("mag-comp, %f, %f, %f\r\n", myCompensatedMag.x, myCompensatedMag.y, myCompensatedMag.z);
        }
    }
    return 0; // never reached
}