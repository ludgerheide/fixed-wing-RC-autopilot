//
//  imuBoard.c
//  avr - flyByWire
//
//  Created by Ludger Heide on 25.09.15.
//  Copyright © 2015 LH Technologies. All rights reserved.
//

#include "imuBoard.h"

#ifdef IMU_DEBUG
#include <stdio.h>
#endif

#include <assert.h>
#include "avrlibdefs.h"
#include "avrlibtypes.h"

#include "accelMag.h"
#include "gyro.h"
#include "bmp.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "i2c.h"
#include "timer.h"

#ifndef CRITICAL_SECTION_START
#define CRITICAL_SECTION_START	unsigned char _sreg = SREG; cli()
#define CRITICAL_SECTION_END	SREG = _sreg
#endif

//Initialize "enabled" as false so the timer ISR (and other methods) don't communicate with the devices until they're ready
volatile imuFlags theFlags = {.gyroEnabled = FALSE, .accelEnabled = FALSE, .magEnabled = FALSE, .bmpEnabled = FALSE};

//Initialize all sensors on the IMU board
void IMUinit() {
    //Initialize the I2C bus
    i2cInit();
    
    //Disable interrupts while everything is being set up
    CRITICAL_SECTION_START;
    
    //Initialize the gyro
    if(gyroInit(GYRO_RANGE_250DPS)) {
        theFlags.gyroEnabled = TRUE;
        
        //Set the pin as input
        cbi(DDRE, DDE4);
        
        //Disable the pullup on the relevant pin
        cbi(PORTE, PORTE4);
        
        //Attach the DRDY interrupt
        sbi(EICRB, ISC40); //Writing 1 to ISC40 and ISC41 sets interrupt on rising edge
        sbi(EICRB, ISC41);
        
        sbi(EIMSK, INT4); //Setting INT4 in EIMSK enables the interrupt (if they are globally enabled (but they are cause i2cinit did that))
        
        //Now wait for DRDY to go low and reenmable it to start everything off
        while(inb(PINE) & _BV(PINE4)) {
            ; //Wait for the port to go low
        }
        gyroEnableDrdy();
    } else {
        #ifdef IMU_DEBUG
        printf("Error intitializing the gyro!\r\n");
        #endif
        theFlags.gyroEnabled = FALSE;
    }
    
    //Intialize the accelerometer
    if(accelInit()) {
        theFlags.accelEnabled = TRUE;
        
        //Set the pin as input
        cbi(DDRD, DDD2);
        
        //Disable the pullup on the relevant pin
        cbi(PORTD, PORTD2);
        
        //Attach the DRDY interrupt
        sbi(EICRA, ISC20); //Writing 1 to ISC20 and ISC21 sets interrupt on rising edge
        sbi(EICRA, ISC21);
        
        sbi(EIMSK, INT2); //Setting INT2 in EIMSK enables the interrupt (if they are globally enabled (but they are cause i2cinit did that))
        
        //Now wait for DRDY to go low and reenable it to start everything off
        while(inb(PINB) & _BV(PINB2)) {
            ; //Wait for the port to go low
        }
        accelEnableDrdy();
    } else {
        #ifdef IMU_DEBUG
        printf("Error intitializing the accelerometer!\r\n");
        #endif
        theFlags.accelEnabled = FALSE;
    }
    
    //Initialize the magnetometer
    if(magInit()) {
        theFlags.magEnabled = TRUE;
        
        //Set the pin as input
        cbi(DDRD, DDD3);
        
        //Disable the pullup on the DRDY pin
        cbi(PORTD, PORTD3);
        
        //Attach the DRDY interrupt
        sbi(EICRA, ISC30); //Writing 1 to ISC30 and ISC31 sets interrupt on rising edge
        sbi(EICRA, ISC31);
        
        sbi(EIMSK, INT3); //Setting INT3 in EIMSK enables the interrupt (if they are globally enabled (but they are cause i2cinit did that))
        
    } else {
        #ifdef IMU_DEBUG
        printf("Error intitializing the magnetometer!\r\n");
        #endif
        theFlags.magEnabled = FALSE;
    }
    
    //Initialize the BMP180 pressure/temp sensor
    if(bmpInit()) {
        theFlags.bmpEnabled = TRUE;
        
        //Attach the interrupt handler for the timer ticks
        timerSetInterruptCallback( imuTimerTick );
        
        //Start a temperature measurrement to set everything off
        const u08 toSend[2] = {BMP180_REGISTER_CONTROL, BMP180_REGISTER_READTEMPCMD};
        i2cMasterSendNI(BMP180_ADDRESS, 2, (u08*)&toSend);
        
        myBmpState = TEMPERATURE_MEASURING;
        bmpLastStateChange = millis();
        
    } else {
        #ifdef IMU_DEBUG
        printf("Error intitializing the BMP180!\r\n");
        #endif
        theFlags.bmpEnabled = FALSE;
    }
    
    
    //Attach the custom stop handler after all sensors are initialized
    i2cSetStopHandler( customStopHandler );
    
    //Re-enable the interrupts
    CRITICAL_SECTION_END;
}

//Prototype for the last step of the following method
static void checkForPendingTransmissionsOrStop(void);

//Gets called when an I2C transmission is over
void customStopHandler(u08 statusReg, u08 deviceaddress) {
    //Set the state to idle so the i2c.c methods work
    I2cState = I2C_IDLE;
    
    //First, find out what just happened
    if(statusReg == TW_MT_DATA_ACK) {
        #ifdef IMU_DEBUG
        printf("SH: Transmission complete\r\n");
        #endif
        
        //Now we should receive data for the correct device
        switch (deviceaddress) {
            case L3GD20_ADDRESS:
                //decrement the DRDY flag for this device
                assert(theFlags.gyroReady);
                theFlags.gyroReady = FALSE;
                
                //The data received from the L3GD20 will be six bytes long
                i2cMasterStartReceiving(L3GD20_ADDRESS, 6);
                break;
                
            case LSM303_ADDRESS_ACCEL:
                //unset the DRDY flag for this device
                assert(theFlags.accelReady);
                theFlags.accelReady = FALSE;
                
                //The data received will be six bytes long
                i2cMasterStartReceiving(LSM303_ADDRESS_ACCEL, 6);
                break;
                
            case LSM303_ADDRESS_MAG:
                //unset DRDY
                assert(theFlags.magReady);
                theFlags.magReady = FALSE;
                
                //The data received will be six bytes long
                i2cMasterStartReceiving(LSM303_ADDRESS_MAG, 6);
                break;
                
            case BMP180_ADDRESS:
                assert(myBmpState == TEMPERATURE_MEASURING || myBmpState == TEMPERATURE_READY || myBmpState == PRESSURE_MEASURING || myBmpState == PRESSURE_READY);
                
                //Check the state
                switch (myBmpState) {
                    case TEMPERATURE_MEASURING:
                        #ifdef IMU_DEBUG
                        printf("BMP: Temp in progress\r\n");
                        #endif
                        
                        //We have not decided what to do on the bus yet, so decide it
                        checkForPendingTransmissionsOrStop();
                        
                        break;
                        
                    case TEMPERATURE_READY:
                        #ifdef IMU_DEBUG
                        printf("BMP: Temp ready\r\n");
                        #endif
                        assert(theFlags.bmpReady);
                        assert(millis() - bmpLastStateChange >= BMP180_TEMPERATURE_DURATION);
                        
                        theFlags.bmpReady = FALSE;
                        
                        //The received data will be two bytes long
                        i2cMasterStartReceiving(BMP180_ADDRESS, 2);
                        myBmpState = TEMPERATURE_RECEIVING;
                        break;
                        
                    case PRESSURE_MEASURING:
                        #ifdef IMU_DEBUG
                        printf("BMP: Pressure in progress\r\n");
                        #endif
                        
                        //We have not decided what to do on the bus yet, so decide it
                        checkForPendingTransmissionsOrStop();
                        
                        break;
                        
                    case PRESSURE_READY:
                        #ifdef IMU_DEBUG
                        printf("BMP: Pressure ready\r\n");
                        #endif
                        
                        assert(theFlags.bmpReady);
                        assert(millis() - bmpLastStateChange >= BMP180_PRESSURE_DURATION);
                        
                        theFlags.bmpReady = FALSE;
                        
                        //The received data will be three bytes (actually 19 bits) long
                        i2cMasterStartReceiving(BMP180_ADDRESS, 3);
                        myBmpState = PRESSURE_RECEIVING;
                        break;
                        
                    default:
                        break;
                }
                break;
                
            default:
                #ifdef IMU_DEBUG
                printf("Invalid address at line %i", __LINE__);
                #endif
                break;
        }
        
    } else if(statusReg == TW_MR_DATA_NACK) {
        #ifdef IMU_DEBUG
        printf("SH: Reception complete\r\n");
        #endif
        
        //First, get the data out of the RX buffer
        switch (deviceaddress) {
            case L3GD20_ADDRESS:
                //The buffer should be 6 bytes long and full
                assert(I2cReceiveDataLength == 6);
                assert(I2cReceiveDataIndex == 6);
                
                myGyroRawData.xlo = I2cReceiveData[0];
                myGyroRawData.xhi = I2cReceiveData[1];
                myGyroRawData.ylo = I2cReceiveData[2];
                myGyroRawData.yhi = I2cReceiveData[3];
                myGyroRawData.zlo = I2cReceiveData[4];
                myGyroRawData.zhi = I2cReceiveData[5];
                
                //We have not decided what to do on the bus yet, so decide it
                checkForPendingTransmissionsOrStop();
                break;
                
            case LSM303_ADDRESS_ACCEL:
                //The buffer should be six bytes long and full
                assert(I2cReceiveDataLength == 6);
                assert(I2cReceiveDataIndex == 6);
                
                myAccelRawData.xlo = I2cReceiveData[0];
                myAccelRawData.xhi = I2cReceiveData[1];
                myAccelRawData.ylo = I2cReceiveData[2];
                myAccelRawData.yhi = I2cReceiveData[3];
                myAccelRawData.zlo = I2cReceiveData[4];
                myAccelRawData.zhi = I2cReceiveData[5];
                
                //We have not decided what to do on the bus yet, so decide it
                checkForPendingTransmissionsOrStop();
                break;
                
            case LSM303_ADDRESS_MAG:
                //The buffer should be six bytes long and full
                assert(I2cReceiveDataLength == 6);
                assert(I2cReceiveDataIndex == 6);
                
                myMagRawData.xhi = I2cReceiveData[0];
                myMagRawData.xlo = I2cReceiveData[1];
                myMagRawData.zhi = I2cReceiveData[2];
                myMagRawData.zlo = I2cReceiveData[3];
                myMagRawData.yhi = I2cReceiveData[4];
                myMagRawData.ylo = I2cReceiveData[5];
                
                //We have not decided what to do on the bus yet, so decide it
                checkForPendingTransmissionsOrStop();
                break;
                
            case BMP180_ADDRESS:
                assert(myBmpState == TEMPERATURE_RECEIVING || myBmpState == PRESSURE_RECEIVING);
                
                switch (myBmpState) {
                    case TEMPERATURE_RECEIVING:
                        #ifdef IMU_DEBUG
                        printf("BMP: Temp finished receiving\r\n");
                        #endif
                        bmpGetTempDataFromI2cBuffer();
                        
                        //After the temperature measurement is done, start a pressure measurement
                        bmpStartPressureCapture();
                        myBmpState = PRESSURE_MEASURING;
                        bmpLastStateChange = millis();
                        break;
                        
                    case PRESSURE_RECEIVING:
                        #ifdef IMU_DEBUG
                        printf("BMP: Temp finished receiving\r\n");
                        #endif
                        bmpGetPressDataFromI2cBuffer();
                        
                        //After the pressure is done, start another temperature measurement
                        bmpStartTemperatureCapture();
                        myBmpState = TEMPERATURE_MEASURING;
                        bmpLastStateChange = millis();
                        break;
                        
                    default:
                        break;
                }
                break;
                
            default:
                #ifdef IMU_DEBUG
                printf("Invalid address at line %i\r\n", __LINE__);
                #endif
                break;
        }
        
    } else {
        #ifdef IMU_DEBUG
        printf("SH: Error!\r\n");
        #endif
        
        //Reset the bus
        // reset internal hardware and release bus
        outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWSTO)|BV(TWEA));
        // set state
        I2cState = I2C_IDLE;
    }
}

static void checkForPendingTransmissionsOrStop() {
    //Find out if there are outstandind transfers
    if(theFlags.accelReady) {
        //Send a data request
        accelSendDataRequest();
#ifdef IMU_DEBUG
        printf("Sending accel data request! @l: %i\r\n", __LINE__);
#endif
    } else if (theFlags.gyroReady) {
        //Send the data request
        gyroSendDataRequest();
#ifdef IMU_DEBUG
        printf("Sending gyro data request! @l: %i\r\n", __LINE__);
#endif
    } else if (theFlags.magReady) {
        //Send a data request
        magSendDataRequest();
#ifdef IMU_DEBUG
        printf("Sending mag data request! @l: %i\r\n", __LINE__);
#endif
    } else if (theFlags.bmpReady) {
        //Assert there is actually something ready
        assert(myBmpState == TEMPERATURE_READY || myBmpState == PRESSURE_READY);
        if(myBmpState == TEMPERATURE_READY) {
#ifdef IMU_DEBUG
            printf("Starting temp data request! @l: %i\r\n", __LINE__);
#endif
            bmpStartReceivingTemperature();
        } else if (myBmpState == PRESSURE_READY) {
#ifdef IMU_DEBUG
            printf("Starting pressure data request! @l: %i\r\n", __LINE__);
#endif
            bmpStartReceivingPressure();
        }
    } else {
#ifdef IMU_DEBUG
        printf("IMU: Sending stop!\r\n");
#endif
        //Nothing to be done. Send a STOP condition
        outb(TWCR, (inb(TWCR)&TWCR_CMD_MASK)|BV(TWINT)|BV(TWEA)|BV(TWSTO));
    }
}

void imuTimerTick(u32 millis) {
    //Only do stuff if the bmp is actually enabled
    if(theFlags.bmpEnabled) {
        switch (myBmpState) {
            case TEMPERATURE_MEASURING:
                if(millis - bmpLastStateChange >= BMP180_TEMPERATURE_DURATION) {
                    #ifdef IMU_DEBUG
                    printf("TCB: Temp ready\r\n");
                    #endif
                    //The measurement is ready
                    myBmpState = TEMPERATURE_READY;
                    theFlags.bmpReady = TRUE;
                    
                    //If I2C is idle, transmit the temp register request, else just set the flag
                    if(I2cState == I2C_IDLE) {
                        bmpStartReceivingTemperature();
                    }
                }
                break;
                
            case PRESSURE_MEASURING:
                if(millis - bmpLastStateChange >= BMP180_PRESSURE_DURATION) {
                    #ifdef IMU_DEBUG
                    printf("TCB: Pressure ready\r\n");
                    #endif
                    //The measurement is ready
                    myBmpState = PRESSURE_READY;
                    theFlags.bmpReady = TRUE;
                    
                    //If I2C is idle, transmit the temp register request, else just set the flag
                    if(I2cState == I2C_IDLE) {
                        bmpStartReceivingPressure();
                    }
                }
                break;
                
            default:
                break;
        }
        
    }
    #ifdef IMU_DEBUG
    else {
        printf("BMP disabled at line %i", __LINE__);
    }
    #endif
    
}

ISR(INT3_vect) {
    //This is the handler for the mag-DRDY interrupt
    #ifdef IMU_DEBUG
    printf("Mag-DRDY received! %lu\r\n", millis());
    #endif
    //Set the flag to true if it isn't already
    if(!theFlags.magReady) {
        theFlags.magReady = TRUE;
    }
    #ifdef IMU_DEBUG
    else {
        printf("WARNING: theflags.magReady at 1!\r\n");
    }
    #endif
    
    //Set the timestamp
    myMagRawData.timestamp = micros();
    
    //Start the transmission or enqueue it
    if(I2cState == I2C_IDLE) {
    #ifdef IMU_DEBUG
        printf("I2C idle, starting tx!\r\n");
    #endif
        //Send a data request
        magSendDataRequest();
    }
    #ifdef IMU_DEBUG
    else {
        
        printf("I2C busy, just setting flag!\r\n");
    }
    #endif
}

ISR(INT2_vect) {
    //This is the external interrupt for accel-DRDY on LIN1
    #ifdef IMU_DEBUG
    printf("Accel-DRDY received!\r\n");
    #endif
    
    //Set the flag to true if it isn't already
    if(!theFlags.accelReady) {
        theFlags.accelReady = TRUE;
    }
    #ifdef IMU_DEBUG
    else {
        printf("WARNING: theflags.accelReady at 1!\r\n");
    }
    #endif
    
    //Set the timestamp
    myAccelRawData.timestamp = micros();
    
    //Start the transmission or enqueue it
    if(I2cState == I2C_IDLE) {
        #ifdef IMU_DEBUG
        printf("I2C idle, starting tx!\r\n");
        #endif
        //Send a data request
        accelSendDataRequest();
    }
    #ifdef IMU_DEBUG
    else {
        
        printf("I2C busy, just setting flag!\r\n");
    }
    #endif
}

ISR(INT4_vect) {
    //This is the external interrupt for GRDY
    #ifdef IMU_DEBUG
    printf("GRDY received!\r\n");
    #endif
    
    //Increment the flag but don't roll it Over
    if(!theFlags.gyroReady) {
        theFlags.gyroReady = TRUE;
    }
    #ifdef IMU_DEBUG
    else {
        printf("WARNING: theflags.gyroReady at 1!\r\n");
    }
    #endif
    
    //Set the timestamp
    myGyroRawData.timestamp = micros();
    
    if(I2cState == I2C_IDLE) {
        #ifdef IMU_DEBUG
        printf("I2C idle, starting tx!\r\n");
        #endif
        //Send the data request
        gyroSendDataRequest();
    }
    #ifdef IMU_DEBUG
    else {
        
        printf("I2C busy, just setting flag!\r\n");
    }
    #endif
}