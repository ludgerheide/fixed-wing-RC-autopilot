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

#include "global.h"
#include <assert.h>

static char encodedMessageBuffer[100];

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

//Initializes the communications (the xBee serial and the RTS output pin)
void commsInit(void) {
    xBeeInit();
}

void commsProcessMessage(char* message, u08 size) {
    assert(size <= 100);
    
    //Zero out the message so we only get the new data
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
    if(incomingMsg.has_command_set) {
        if(incomingMsg.command_set.yaw <= INT16_MAX && incomingMsg.command_set.yaw >= INT16_MIN) {
            //We have a valid yaw
            commandSet.yaw = incomingMsg.command_set.yaw;
        }
        #ifdef COMMS_DEBUG
        else {
            printf("Yaw out of bounds! %i", __LINE__);
        }
        #endif
        
        if(incomingMsg.command_set.pitch <= INT16_MAX && incomingMsg.command_set.pitch >= INT16_MIN) {
            //We have a valid pitch
            commandSet.pitch = incomingMsg.command_set.pitch;
        }
        #ifdef COMMS_DEBUG
        else {
            printf("Pitch out of bounds! %i", __LINE__);
        }
        #endif
        
        if(incomingMsg.command_set.roll <= INT16_MAX && incomingMsg.command_set.roll >= INT16_MIN) {
            //We have a valid pitch
            commandSet.roll = incomingMsg.command_set.roll;
        }
        #ifdef COMMS_DEBUG
        else {
            printf("Roll out of bounds! %i", __LINE__);
        }
        #endif
    }
}

void commsSendStatusReport(void) {
    //Create a new message
    DroneMessage outgoingMsg = DroneMessage_init_zero;
    
    //Fill in the data
    outgoingMsg.has_current_position = true;
    outgoingMsg.current_position.timestamp = currentPosition.timestamp;
    outgoingMsg.current_position.latitude = degreesToInt(currentPosition.latitude);
    outgoingMsg.current_position.longitude = degreesToInt(currentPosition.longitude);
    outgoingMsg.current_position.altitude = currentPosition.altitude;
    
    outgoingMsg.has_current_speed = true;
    outgoingMsg.current_speed.timestamp = currentVelocity.timestamp;
    outgoingMsg.current_speed.course_over_ground = currentVelocity.courseOverGround;
    outgoingMsg.current_speed.speed = currentVelocity.speed;
    
    outgoingMsg.has_current_attitude = true;
    outgoingMsg.current_attitude.timestamp = attitude.timestamp;
    outgoingMsg.current_attitude.course_magnetic = degreesToInt(attitude.courseMagnetic);
    outgoingMsg.current_attitude.pitch = degreesToInt(attitude.pitch);
    outgoingMsg.current_attitude.roll = degreesToInt(attitude.roll);
    
    
    pb_ostream_t stream = pb_ostream_from_buffer((u08*)encodedMessageBuffer, sizeof(encodedMessageBuffer));
    
    /* Now we are ready to encode the message! */
    bool status = pb_encode(&stream, DroneMessage_fields, &outgoingMsg);
    u08 message_length = stream.bytes_written;
    assert(message_length <= 100);
    
    /* Then just check for any errors.. */
    if (!status)
    {
        #ifdef COMMS_DEBUG
        printf("Encoding failed: %s\n", PB_GET_ERROR(&stream));
        #endif
        return;
    } else {
        //transmit the message
        xBeeSendPayload(encodedMessageBuffer, message_length, false, 0);
    }
}