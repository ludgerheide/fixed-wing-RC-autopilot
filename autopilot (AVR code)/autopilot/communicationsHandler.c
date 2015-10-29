//
//  communicationsHandler.c
//  autopilot
//
//  Created by Ludger Heide on 06.10.15.
//  Copyright © 2015 Ludger Heide. All rights reserved.
//

#include "communicationsHandler.h"
#include "communicationProtocol.pb.h"

#include "pb_encode.h"
#include "pb_decode.h"
#include "pb_common.h"
#include "buffer.h"

#include "nmea.h"
#include "bmp.h"

#include "pinSetup.h"
#include "uart4.h"
#include "timer.h"
#include "global.h"
#include <assert.h>
#include <math.h>

#define KMH_TO_CMS (1.0/0.036)

typedef enum {
    telemetry,
    logging
}messagePurpose;

static char messageBuffer[255];

//This holds the ID and time of the last transmission. We use it to refreain from sending until the last one has gone through
//As well as limiting the telemetry frequency
static BOOL lastTxAcked;
static u32 lastTxTime;
const static u16 telemetryDelay = 333; //The delay between each telemetry message in milliseconds

//These hold the timestamps for the last packets that were
// a) transmitted over the telemetry
// b) send thtough the serial (logging port)
static u32 telPosTime, telVelTime, telAttitudeTime;
static u32 logPosTime, logVelTime, logAttitudeTime, logBaroDataTime, logRawGyroTime, logRawAccelTime, logRawMagTime, logHomeBaseTime, logWaypointTime, logInCommandSetTime, logOutCommandSetTime;

//Function to convert degrees to a fixed-point 16 byte integer
static s16 degreesToInt(float degrees) {
    //360 degrees stays in the 16-bit bounds if right shifted by six bits
    // Or multiplied by 64
    
    return (s16)(degrees * 64.0);
}

//Converts a fixed-point interger to a float
static float intToDegrees(s16 fixedPoint) {
    //360 degrees stays in the 16-bit bounds if right shifted by six bits
    // Or multiplied by 64
    
    return (float)(fixedPoint / 64.0);
}

//Function n that creates a message and puts it into the message buffer
static u08 createProtobuf(messagePurpose thePurpose, u08* messageLength) {
    assert(thePurpose == logging || thePurpose == telemetry);
    
    //Logging gets everything we have, telemtry gets less
    //Create a new message
    DroneMessage outgoingMsg = DroneMessage_init_zero;
    
    //Check if we have new messages
    
    //FlighMode just for logging, but then always
    if(thePurpose == logging) {
        outgoingMsg.has_current_mode = true;
        outgoingMsg.current_mode = currentFlightMode;
    }
    
    //Position and velocity for both, but with the correct last update
    if((thePurpose == logging && GpsInfo.PosLLA.timestamp - logPosTime) || (thePurpose == telemetry && GpsInfo.PosLLA.timestamp - telPosTime)) {
        outgoingMsg.has_current_position = true;
        
        //For logging, add real time and timestamp
        if(thePurpose == logging) {
            outgoingMsg.current_position.has_timestamp = true;
            outgoingMsg.current_position.timestamp = GpsInfo.PosLLA.timestamp;
            
            outgoingMsg.current_position.has_real_time = true;
            outgoingMsg.current_position.real_time = GpsInfo.PosLLA.TimeOfFix;
        }
        outgoingMsg.current_position.latitude = GpsInfo.PosLLA.lat;
        outgoingMsg.current_position.longitude = GpsInfo.PosLLA.lon;
        outgoingMsg.current_position.has_altitude = true;
        outgoingMsg.current_position.altitude = pressureToAltitude(curPressure.pressure, seaLevelPressure) * 100; //To convert to cventimeters
        
        //Now update the time
        if(thePurpose == logging) {
            logPosTime = GpsInfo.PosLLA.timestamp;
        } else {
            telPosTime = GpsInfo.PosLLA.timestamp;
        }
    }
    
    //Velocity for both, as above
    if((thePurpose == logging && GpsInfo.VelHS.timestamp - logVelTime) || (thePurpose == telemetry && GpsInfo.PosLLA.timestamp - telVelTime)) {
        outgoingMsg.has_current_speed = true;
        
        //For logging, with timestamp
        if(thePurpose == logging) {
            outgoingMsg.current_speed.has_timestamp = true;
            outgoingMsg.current_speed.timestamp = GpsInfo.VelHS.timestamp;
        }
        
        outgoingMsg.current_speed.speed = GpsInfo.VelHS.speed * 1000; //in m/h
        outgoingMsg.current_speed.course_over_ground = degreesToInt(GpsInfo.VelHS.heading);
        
        //Now update the time
        if(thePurpose == logging) {
            logVelTime = GpsInfo.VelHS.timestamp;
        } else {
            telVelTime = GpsInfo.VelHS.timestamp;
        }
    }
    
    //Attitude for both, as above
    if((thePurpose == logging && currentAttitude.timestamp - logAttitudeTime) || (thePurpose == telemetry && currentAttitude.timestamp - telAttitudeTime)) {
        outgoingMsg.has_current_attitude = true;
        
        //For logging with timestamp
        if(thePurpose == logging) {
            outgoingMsg.current_attitude.has_timestamp = true;
            outgoingMsg.current_attitude.timestamp = currentAttitude.timestamp;
        }
        
        outgoingMsg.current_attitude.course_magnetic = degreesToInt(currentAttitude.courseMagnetic);
        outgoingMsg.current_attitude.pitch = degreesToInt(currentAttitude.pitch);
        outgoingMsg.current_attitude.roll = degreesToInt(currentAttitude.roll);
        
        //Now update the time
        if(thePurpose == logging) {
            logAttitudeTime = currentAttitude.timestamp;
        } else {
            telAttitudeTime = currentAttitude.timestamp;
        }
    }
    
    //Barodata just for logging
    if(thePurpose == logging && curPressure.timestamp - logBaroDataTime) {
        outgoingMsg.has_bmp_raw = true;
        
        outgoingMsg.bmp_raw.timestamp = curPressure.timestamp;
        outgoingMsg.bmp_raw.pressure = curPressure.pressure;
        outgoingMsg.bmp_raw.temperature = curPressure.temperature;
        
        //Now update the time
        logBaroDataTime = curPressure.timestamp;
    }
    
    //Gyrodata just for logging
    if(thePurpose == logging && curGyro.timestamp - logRawGyroTime) {
        outgoingMsg.has_gyro_raw = true;
        
        outgoingMsg.gyro_raw.timestamp = curGyro.timestamp;
        outgoingMsg.gyro_raw.x = curGyro.x;
        outgoingMsg.gyro_raw.y = curGyro.y;
        outgoingMsg.gyro_raw.z = curGyro.z;
        
        //Now update the time
        logRawGyroTime = curGyro.timestamp;
    }
    
    //MagData just for logging
    if(thePurpose == logging && curMag.timestamp - logRawMagTime) {
        outgoingMsg.has_mag_raw = true;
        
        outgoingMsg.mag_raw.timestamp = curMag.timestamp;
        outgoingMsg.mag_raw.x = curMag.x;
        outgoingMsg.mag_raw.y = curMag.y;
        outgoingMsg.mag_raw.z = curMag.z;
        
        //Time update
        logRawMagTime = curMag.timestamp;
    }
    
    //AccelData just for logging
    if(thePurpose == logging && curAccel.timestamp - logRawAccelTime) {
        outgoingMsg.has_accel_raw = true;
        
        outgoingMsg.accel_raw.timestamp = curAccel.timestamp;
        outgoingMsg.accel_raw.x = curAccel.x;
        outgoingMsg.accel_raw.y = curAccel.y;
        outgoingMsg.accel_raw.z = curAccel.z;
        
        //Time update
        logRawAccelTime = curAccel.timestamp;
    }
    
    //In logging mode, also log inputs
    //waypoint
    if(thePurpose == logging && currentTarget.timestamp - logWaypointTime) {
        outgoingMsg.has_waypoint = true;
        
        outgoingMsg.waypoint.has_timestamp = true;
        outgoingMsg.waypoint.timestamp = currentTarget.timestamp;
        
        outgoingMsg.waypoint.latitude = currentTarget.latitude;
        outgoingMsg.waypoint.longitude = currentTarget.latitude;
        
        outgoingMsg.waypoint.has_altitude = true;
        outgoingMsg.waypoint.altitude = currentTarget.altitude * 100;
        
        //Time update
        logWaypointTime = currentTarget.timestamp;
    }
    
    //home base
    if(thePurpose == logging && homeBase.timestamp - logHomeBaseTime) {
        outgoingMsg.has_home_base = true;
        
        outgoingMsg.home_base.has_timestamp = true;
        outgoingMsg.home_base.timestamp = homeBase.timestamp;
        
        outgoingMsg.home_base.latitude = homeBase.latitude;
        outgoingMsg.home_base.longitude = homeBase.longitude;
        
        outgoingMsg.home_base.has_altitude = true;
        outgoingMsg.home_base.altitude = homeBase.altitude * 100;
        
        //Time update
        logHomeBaseTime = homeBase.timestamp;
    }
    
    //SLP
    if(thePurpose == logging) {
        outgoingMsg.has_sea_level_pressure = true;
        outgoingMsg.sea_level_pressure = seaLevelPressure;
    }
    
    //Input Command set
    if(thePurpose == logging && inputCommandSet.timestamp - logInCommandSetTime) {
        outgoingMsg.has_input_command_set = true;
        
        outgoingMsg.input_command_set.yaw = inputCommandSet.yaw;
        outgoingMsg.input_command_set.pitch = inputCommandSet.pitch;
        outgoingMsg.input_command_set.thrust = inputCommandSet.thrust;
        
        //Time update
        logInCommandSetTime = inputCommandSet.timestamp;
    }
    
    //Output Command set
    if(thePurpose == logging && outputCommandSet.timestamp - logOutCommandSetTime) {
        outgoingMsg.has_output_command_set = true;
        
        outgoingMsg.output_command_set.yaw = outputCommandSet.yaw;
        outgoingMsg.output_command_set.pitch = outputCommandSet.pitch;
        outgoingMsg.output_command_set.thrust = outputCommandSet.thrust;
        
        //Time update
        logOutCommandSetTime = outputCommandSet.timestamp;
    }
    
    //Now create the buffer and write the message out
    pb_ostream_t stream = pb_ostream_from_buffer((u08*)messageBuffer, sizeof(messageBuffer));

    /* Now we are ready to encode the message! */
    bool status = pb_encode(&stream, DroneMessage_fields, &outgoingMsg);
    *messageLength = stream.bytes_written;
    
    #ifdef COMMS_DEBUG
    printf("Created message of size %i for purpose %i \r\n", *messageLength, thePurpose);
    #endif
    
    /* Then just check for any errors.. */
    if (!status)
    {
        #ifdef COMMS_DEBUG
        printf("Encoding failed: %s\r\n", PB_GET_ERROR(&stream));
        #endif
        return 1;
    }
    return 0;
}

//Initializes the communications (the xBee serial and the RTS output pin)
void commsInit(void) {
    xBeeInit();
    xBeeAttachTxStatusHandler( txStatusHandler );
}

void commsProcessMessage(char* message, u08 size) {
    #ifdef COMMS_DEBUG
    printf("Processing msg!");
    #endif
    
    assert(size <= 100);
    u32 now = millis();
    
    //Zero out the message so we only get valid data
    DroneMessage incomingMsg = DroneMessage_init_zero;
    
    //Set up an input stream
    pb_istream_t inStream = pb_istream_from_buffer((u08*)message, size);
    
    BOOL status = pb_decode(&inStream, DroneMessage_fields, &incomingMsg);
    
    if(!status) {
        #ifdef COMMS_DEBUG
        printf("Decoding failed!");
        #endif
        return;
    }

    //Now do stuff according to the content of the decoded message
    if(incomingMsg.has_input_command_set) {
        inputCommandSet.timestamp = now;
        
        if(incomingMsg.input_command_set.yaw <= UINT8_MAX && incomingMsg.input_command_set.yaw >= 0) {
            //We have a valid yaw
            inputCommandSet.yaw = incomingMsg.input_command_set.yaw;
        }
        #ifdef COMMS_DEBUG
        else {
            printf("Yaw out of bounds! %i", __LINE__);
        }
        #endif
        
        if(incomingMsg.input_command_set.pitch <= UINT8_MAX && incomingMsg.input_command_set.pitch >= 0) {
            //We have a valid pitch
            inputCommandSet.pitch = incomingMsg.input_command_set.pitch;
        }
        #ifdef COMMS_DEBUG
        else {
            printf("Pitch out of bounds! %i", __LINE__);
        }
        #endif
        
        if(incomingMsg.input_command_set.thrust <= UINT8_MAX && incomingMsg.input_command_set.thrust >= 0) {
            //We have a valid thrust
            inputCommandSet.thrust = incomingMsg.input_command_set.thrust;
        }
        #ifdef COMMS_DEBUG
        else {
            printf("Roll out of bounds! %i", __LINE__);
        }
        #endif
    }
    
    if(incomingMsg.has_sea_level_pressure) {
        seaLevelPressure = incomingMsg.sea_level_pressure;
    }
    
    if(incomingMsg.has_waypoint) {
        currentTarget.timestamp = now;
        
        currentTarget.latitude = incomingMsg.waypoint.latitude;
        currentTarget.longitude = incomingMsg.waypoint.longitude;
        if(incomingMsg.waypoint.has_altitude) {
            currentTarget.altitude = incomingMsg.waypoint.altitude / 100.0;
        } else {
            currentTarget.altitude = NAN;
        }
    }
    
    if(incomingMsg.has_home_base) {
        homeBase.timestamp = now;
        
        homeBase.latitude = incomingMsg.home_base.latitude;
        homeBase.longitude = incomingMsg.home_base.longitude;
        if(incomingMsg.home_base.has_altitude) {
            homeBase.altitude = incomingMsg.home_base.altitude / 100.0;
        } else {
            homeBase.altitude = NAN;
        }
    }
    
    if(incomingMsg.has_new_mode) {
        currentFlightMode = incomingMsg.new_mode;
    }
}

void commsCheckAndSendTelemetry(void) {
    u32 now = millis();
    u08 telemetryLength;
    
    //Check if the serial buffer is empty, we should sens a new msg
    //and the last transmission has been acked (disreagars the ack check if more than one second has passed since the last transmission
    if(uartReadyTx[XBEE_UART] && (now - lastTxTime > telemetryDelay) && (lastTxAcked || now - lastTxTime > 1000)) {
        if(createProtobuf(telemetry, &telemetryLength)) {
            #ifdef COMMS_DEBUG
            printf("Creating protobuf failed @ %i\r\n", __LINE__);
            #endif
            return;
        } else {
            xBeeSendPayload(messageBuffer, telemetryLength, false, 0xAB);
            lastTxAcked = FALSE;
            lastTxTime = now;
        }
    }
}

void commsCheckAndSendLogging(void) {
    u08 loggingLength = 0;
    
    //Check if the serial buffer is empty
    if(uartReadyTx[RASPI_UART]) {
        if(createProtobuf(logging, &loggingLength)) {
            //Nonzero return code indicates failure
            #ifdef COMMS_DEBUG
            printf("Creating protobuf failed @ %i\r\n", __LINE__);
            #endif
            return;
        }
        
        //If we get here creating the protobuf succeeded. Put the magic number in the buffer first, then the length, then the message
        uartAddToTxBuffer(RASPI_UART, 's');
        uartAddToTxBuffer(RASPI_UART, 't');
        uartAddToTxBuffer(RASPI_UART, 'a');
        uartAddToTxBuffer(RASPI_UART, 'r');
        uartAddToTxBuffer(RASPI_UART, 't');
        uartAddToTxBuffer(RASPI_UART, loggingLength);
        
        u08 checksum = 0;
        //Add the data and add a checksum
        for(u08 i = 0; i < loggingLength; i++) {
            uartAddToTxBuffer(RASPI_UART, messageBuffer[i]);
            checksum += messageBuffer[i];
        }
        uartAddToTxBuffer(RASPI_UART, checksum);
        #ifdef COMMS_DEBUG
        for(u08 i = 0; i < uartGetTxBuffer(RASPI_UART)->datalength; i++) {
            printf("%02x ", uartGetTxBuffer(RASPI_UART)->dataptr[i]);
        }
        printf("\r\n");
        #endif
        uartSendTxBuffer(RASPI_UART);
    }
}

void txStatusHandler(uint8_t frameID, uint8_t retryCount, uint8_t txStatus) {
    if(frameID == 0xAB) {
        #ifdef COMMS_DEBUG
        printf("Tx Ack for fram %02X, retryCount %02X, status %02X\r\n", frameID, retryCount, txStatus);
        #endif
        lastTxAcked = TRUE;
    }
    #ifdef COMMS_DEBUG
    else{
        printf("Invalid msg @%i\r\n", __LINE__);
    }
    #endif
}