/*
TO IMPLEMENT:
  -MAYBE: In setup, make a baseline variable to dynamically update neutral on the sticks. 
  -Interrupt based sensor/joystick checking
  -Emergency stop function
*/
#include <Arduino.h>
#include <stdint.h>
#include <HCSR04.h>
#include <math.h>

//Conversion constant to read distance measure in inches
#define CMCONVERSION 0.393701

//Northwest Sensor
#define nwTrig 22
#define nwEcho 23
//North Sensor
#define nTrig 24
#define nEcho 25
//Northeast Sensor
#define neTrig 26
#define neEcho 27
//Southeast Sensor
#define seTrig 28
#define seEcho 29
//South Sensor
#define sTrig 30
#define sEcho 31
//Southwest Sensor
#define swTrig 32
#define swEcho 33

// Motor A connections
#define enA 5
#define in1 7
#define in2 6
// Motor B connections
#define enB 2
#define in3 4
#define in4 3

// Joystick Pins
#define xpin 0
#define ypin 1

// Ratio of max joystick value to the max motor power value
#define joystickRatio 0.56792 

//C++ class definitions so each sensor exists as it's own object
//Object oriented programing is hard I am not entirely sure what's happening here
HCSR04 nw(nwTrig, nwEcho);
HCSR04 n(nTrig, nEcho);
HCSR04 ne(neTrig, neEcho);
HCSR04 se(seTrig, seEcho);
HCSR04 s(sTrig, sEcho);
HCSR04 sw(swTrig, swEcho);

//Macros 
//Any place where the defined macro is called it will replace that name with the lines of code below
#define SETUPSENSORPIN(d)   pinMode(d##Trig, OUTPUT);\
  pinMode(d##Echo, INPUT);\
  digitalWrite(d##Trig, LOW);

#define SETUPMOTORPIN(d)   pinMode(d, OUTPUT);

#define SETPOWER(d)  digitalWrite(d, LOW);

//Stop function spools down motors 
void stop(void){
  
  for (int i = 255; i >= 0; i-=15) {
		analogWrite(enA, i);
		analogWrite(enB, i);
		delay(20);
	}
  
}

//Run code
void run(void){
  //Local variable initialization 
  double yval = 0;
  double xval = 0;
  //int16_t is an int variable but a defined length, will avoid any potential issues with running the code on 
  //different operating systems
  int16_t leftTurn = 0;
  int16_t rightTurn = 0;
  float nwDistance;
  float nDistance;
  float neDistance;
  float seDistance;
  float sDistance;
  float swDistance;

  //Calling the objects initialized previously to check the distance read on each sensor then multiplying by the conversion
  //to get inches from centimeters
  nwDistance = (nw.dist() * CMCONVERSION);
  nDistance = (n.dist() * CMCONVERSION);
  neDistance = (ne.dist() * CMCONVERSION);
  seDistance = (se.dist() * CMCONVERSION);
  sDistance = (s.dist() * CMCONVERSION);
  swDistance = (sw.dist() * CMCONVERSION);

  //When testing motors or testing without sensors keep this code uncommented
  //Comment out to test specific sensors
  nwDistance = 13;
  nDistance = 13;
  neDistance = 13;
  seDistance = 13;
  sDistance = 13;
  swDistance = 13;

  //Getting values from joystick pins
  yval = analogRead(ypin);
  xval = analogRead(xpin);

  //Converting y and x pin values to coordinates from -255 to 255
  //This code has fine tuned values that would need to be changed on a car by car basis. 
  int16_t ycoordinate = (yval - 511) * joystickRatio;
  int16_t xcoordinate = (xval - 503) * joystickRatio;

  //Checking the x coordinate from the joystick to see if hte user wants to turn
  if (xcoordinate < -20){
    leftTurn = 0;
    rightTurn = abs(xcoordinate) - 25;
  }
  if (xcoordinate > 20){
    leftTurn = abs(xcoordinate) - 10;
    rightTurn = 0;
  }
  
  if (ycoordinate < -15){
    //Checking if the back sensors are blocked while going backwards
    if (swDistance < 12 || sDistance < 12 || seDistance < 12){
        stop();
    }else{
        //Power for the motors is the y coordinate minus the modifier if the user wants to turn
        //If the joystick is at (255,255) in theory the right motor will be powered to 0 and the left to 255
        analogWrite(enA, abs(ycoordinate) - abs(rightTurn));
        analogWrite(enB, abs(ycoordinate) - abs(leftTurn));
        digitalWrite(in1, LOW);
        digitalWrite(in2, HIGH);
        digitalWrite(in3, LOW);
        digitalWrite(in4, HIGH);
    } 
  } else if (ycoordinate > 15){
    //Checking if the front sensors are blocked while driving forward
    if (nwDistance < 12 || nDistance < 12 || neDistance < 12){
        stop();
    }else{
        //Same thing here as before
        analogWrite(enA, abs(ycoordinate) - abs(rightTurn));
        analogWrite(enB, abs(ycoordinate) - abs(leftTurn));
        digitalWrite(in1, HIGH);
        digitalWrite(in2, LOW);
        digitalWrite(in3, HIGH);
        digitalWrite(in4, LOW); 
    }
  } else{
    //If the joystick is neutral then turn off power
    analogWrite(enA, 0);
    analogWrite(enB, 0);
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    digitalWrite(in3, LOW);
    digitalWrite(in4, LOW);
  }
}

void setup() {
  //All the macros make this section way less annoying to write
  SETUPSENSORPIN(nw)
  SETUPSENSORPIN(n)
  SETUPSENSORPIN(ne)
  SETUPSENSORPIN(se)
  SETUPSENSORPIN(s)
  SETUPSENSORPIN(sw)

  SETUPMOTORPIN(enA)
  SETUPMOTORPIN(enB)
  SETUPMOTORPIN(in1)
  SETUPMOTORPIN(in2)
  SETUPMOTORPIN(in3)
  SETUPMOTORPIN(in4)

  SETPOWER(in1)
  SETPOWER(in2)
  SETPOWER(in3)
  SETPOWER(in4)

  Serial.begin(9600);
}

void loop() {
  run();
}


