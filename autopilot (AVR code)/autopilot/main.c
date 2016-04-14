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
#include "raspiComms.h"
#include "servo.h"
#include "battery.h"

#include "flightControllerTypes.h"
#include "flightController.h"

#include "uart4.h"

void initIO(void) {
    raspiInit();
    printfAttachToUart();
    readSlpFromEEPROM();
    readHomeBaseFromEEPROM();
    servoInit();
    timerInit();
    IMUinit();
    gpsInit();
    commsInit();
    batteryInit();
    flightControllerInit();
}

int main(void) __attribute__ ((noreturn));
int main(void) {
    initIO();
    
    while (1) {
        //Get the current sensor events
        gyroGetData(&curGyro);
        accelGetData(&curAccel);
        
        magEvent uncompensatedMag;
        magGetData(&uncompensatedMag);
        magCompensate(&uncompensatedMag, &curMag);
        
        bmpGetData(&curPressure);
        batteryGetData(&curBattery);
        
        //Check if we have a new GPS and xbee available
        if(xBeeNewMessageReady) {
            xBeeHandleMessage();
        }
        if(raspiNewMessageReady) {
            raspiHandleMessage();
        }
        if(gpsCheck()) {
            gpsUpdate();
        }
        
        //Update the madgwick algorithm
        MadgwickAHRSupdate(micros(), curGyro.x, curGyro.y, curGyro.z, curAccel.x, curAccel.y, curAccel.z, curMag.x, curMag.y, curMag.z);
        getYawPitchRollDegrees(&currentAttitude.courseMagnetic, &currentAttitude.pitch, &currentAttitude.roll);
        currentAttitude.timestamp = millis();
        
        //Analyse if we are using expired or invalid data and (TODO) try to reset the sensors
        currentFlightMode = checkSensorsAndSetFlightMode();
        updateFlightControls();
        
        servoSetPosition(YAW_SERVO_CHAN, outputCommandSet.yaw);
        servoSetPosition(PITCH_SERVO_CHAN, outputCommandSet.pitch);
        servoSetPosition(THRUST_SERVO_CHAN, outputCommandSet.thrust);
        
        //Send telemetry
        commsCheckAndSendTelemetry();
        commsCheckAndSendLogging();
    }
}