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
        //Get the current sensor events
        gyroGetData(&curGyro);
        accelGetData(&curAccel);
        magGetData(&curMag);
        bmpGetData(&curPressure);
        
        //Check if we have a new GPS and xbee available
        if(xBeeNewMessageReady) {
            xBeeHandleMessage();
        }
        if(gpsCheck()) {
            gpsUpdate();
        }
        
        //TODO: Analyse if we are using expired or invalid data and try to reset the sensors
        
        
        //Update the madgwick algorithm
        MadgwickAHRSupdate(millis(), curGyro.x, curGyro.y, curGyro.z, curAccel.x, curAccel.y, curAccel.z, curMag.x, curMag.y, curMag.z);
        getYawPitchRollDegrees(&currentAttitude.courseMagnetic, &currentAttitude.pitch, &currentAttitude.roll);
        currentAttitude.timestamp = millis();
        
        //Send telemetry
        commsCheckAndSendTelemetry();
        
        //TODO: Send logging
        
        
        //TODO: Set servos
        
    }
    return 0; // never reached
}