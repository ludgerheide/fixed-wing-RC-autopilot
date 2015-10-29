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
#include "servo.h"

#include "uart4.h"

void initIO(void) {
    servoInit();
    printfAttachToUart();
    timerInit();
    IMUinit();
    gpsInit();
    commsInit();
    
    //Initialize the output command set
    outputCommandSet.yaw = 127;
    outputCommandSet.pitch = 127;
    outputCommandSet.thrust = 0;
    
    _delay_ms(1000);
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
//        commsCheckAndSendLogging();
        
        // Set servos       
        if((inputCommandSet.timestamp) - (outputCommandSet.timestamp)) {
            outputCommandSet.timestamp = millis();
            outputCommandSet.yaw = inputCommandSet.yaw;
            outputCommandSet.pitch = inputCommandSet.pitch;
            outputCommandSet.thrust = inputCommandSet.thrust;
        }
        
        servoSetPosition(YAW_SERVO_CHAN, outputCommandSet.yaw);
        servoSetPosition(PITCH_SERVO_CHAN, outputCommandSet.pitch);
        servoSetPosition(THRUST_SERVO_CHAN, outputCommandSet.thrust);
    }
    return 0; // never reached
}