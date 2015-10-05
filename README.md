# Autopilot for a fixed-wing, three-axis RC plane

This code should become an autopilot (waypoint-following or fly-by-wire with gust compensation) for an RC plane someday.

##Hardware used

* ATMEGA2560 microcontroller
* NMEA GPS (Adafruit ultimate GPS used)
* L3GD20 Gyro (Adafruit 10DOF imu board used)
* LSM303DLHC Magnetomenter/Accelerometer (also on adafruit 10dof)
* BMP180 barometer/altimeter (also on the 10DOF board)
* xBee-API compatible transceiver (xBee PRO 868 used)

## Software requirements

* avr-gcc and avr-libc
* Google's protobuf (version 2.6.1 used)
* nanopb

## Acknowledgements

This project contains code from the following sources:
* [The FreeIMU project](http://www.varesano.net/projects/hardware/FreeIMU)
* Madgwick's AHRS algorithm
* [Procyon AVRlib](http://www.procyonengineering.com/embedded/avr/avrlib/)
* Adafruit's various Arduino libraries for the 10DOF board