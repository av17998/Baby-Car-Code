### Go-Baby-Car Code 

#Code for Baby Car Project for AATL

## Program Description

The current itteration of this code is defining six ultrasonic sensors, using a HCSR04 Library provided on the Arduino website and reading joystick values from the x and y pins of the joystick. Each sensor is interfaced with using one echo pin for each sensor and a trigger pin shared between three sensors, the north and south bank. The joystick outputs values between 0 and 1000, so the values are scaled to fit within the range -255 to 255. With these values, the value is checked to see if the joystick is going forward or backwards, which will check the sensors of the coresponding direction. If the sensors are clear, then the motors will be powerd by the scaled joystick value. Each motor also has a turn left or turn right modifier which will lessen power on each motor depending on if the joystick x value. 

## Hat Board

The main branch is designed to work with the current board design, linked below.
https://github.com/av17998/ArduinoBoard

All libraries have licences included in source files. 

Property of Purdue Polytechnic Institute 
