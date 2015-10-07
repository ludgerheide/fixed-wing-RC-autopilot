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
    
    u32 lastAHRSUpdate = 0;
    const u08 AHRS_delay = 1000/50;
    
    u32 lastStatusSend = 0;
    const u16 status_delay = 100;
    
    while (1) {
        if(gpsCheck()) {
            printf("new sentences @%lu\r\n", millis());
            gpsUpdate();
            
            currentPosition.timestamp = millis();
            
            currentPosition.latitude = GpsInfo.PosLLA.lat;
            currentPosition.longitude = GpsInfo.PosLLA.lon;
            
            pressureEvent myPress;
            bmpGetData(&myPress);
            
            currentPosition.altitude = pressureToAltitude(myPress.pressure, 1010);
            
            currentVelocity.timestamp = millis();
            currentVelocity.speed = GpsInfo.VelHS.speed * 3600; //To cm/s
        }
        
        if((millis() - lastAHRSUpdate) > AHRS_delay) {
            lastAHRSUpdate = millis();
            
            gyroEvent myGyro;
            gyroGetData(&myGyro);
            
            accelEvent myAccel;
            accelGetData(&myAccel);
            
            magEvent myMag, myCompensatedMag;
            magGetData(&myMag);
            magCompensate(&myMag, &myCompensatedMag);
            
            MadgwickAHRSupdate(myGyro.x, myGyro.y, myGyro.z, myAccel.x, myAccel.y, myAccel.z, myCompensatedMag.x, myCompensatedMag.y, myCompensatedMag.z);
            
            float yaw, pitch, roll;
            
            getYawPitchRollRad(&yaw, &pitch, &roll);
            
            attitude.timestamp = millis();
            
            attitude.courseMagnetic = yaw * 180/PI;
            attitude.pitch = pitch * 180/PI;
            attitude.roll = roll * 180/PI;
            
            
            printf("%3.2f\r\n", attitude.courseMagnetic);
        }
        
        if((millis() - lastStatusSend) > status_delay) {
            lastStatusSend = millis();
            
            commsSendStatusReport();
        }
    }
    return 0; // never reached
}