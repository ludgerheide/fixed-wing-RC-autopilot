//
//  pinSetup.h
//  avr - flyByWire
//
//  Created by Ludger Heide on 23.09.15.
//  Copyright © 2015 LH Technologies. All rights reserved.
//

#ifndef pinSetup_h
#define pinSetup_h

//UART Numbers and ther functions
#define XBEE_UART 3
#define GPS_UART 2
#define RASPI_UART 0

//The Servo numbers
#define SERVO1_PORT PORTB
#define SERVO2_PORT PORTB
#define SERVO3_PORT PORTB

#define YAW_SERVO_PORT PORTB
#define YAW_SERVO_DDR DDRB
#define YAW_SERVO_PIN 6
#define YAW_SERVO_CHAN 0

#define PITCH_SERVO_PORT PORTB
#define PITCH_SERVO_DDR DDRB
#define PITCH_SERVO_PIN 5
#define PITCH_SERVO_CHAN 1

#define THRUST_SERVO_PORT PORTL
#define THRUST_SERVO_DDR DDRL
#define THRUST_SERVO_PIN 6
#define THRUST_SERVO_CHAN 2

#endif /* pinSetup_h */