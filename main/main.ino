//This file is to test is using one common trigger pin for all 6 sensors

#include <Arduino.h>
#include <HCSR04.h>
#include <stdint.h>
#include <math.h>

#define CMCONVERSION 0.393701

#define trig 2
//Northwest Sensor
#define nwEcho 5
//North Sensor
#define nEcho 6
//Northeast Sensor
#define neEcho 27
//Southeast Sensor
#define seEcho 8
//South Sensor
#define sEcho 9
//Southwest Sensor
#define swEcho 10

// Motor A connections
#define enA 40
#define in1 34
#define in2 36
// Motor B connections
#define enB 37
#define in3 39
#define in4 38

// Joystick Pins
#define xpin 0
#define ypin 1

// Ratio of max joystick value to the max motor power value
#define joystickRatio 0.56792

//The main difference of the branches is here. By using a different class constructor, we can use one common trigger pin
//for all the sensors rather than two dedicated pins for every sensor. 12 pins becomes 7, making the board design simpler
int pins[6] = {nwEcho, nEcho, neEcho, seEcho, sEcho, seEcho};
HCSR04 sensors(trig, pins, 6);

#define SETUPMOTORPIN(d)   pinMode(d, OUTPUT);

#define SETPOWER(d)  digitalWrite(d, LOW);

void setup(){ 
    pinMode(trig, OUTPUT);
    digitalWrite(trig, LOW);

    pinMode(nwEcho, INPUT);
    pinMode(nEcho, INPUT);
    pinMode(neEcho, INPUT);
    pinMode(seEcho, INPUT);
    pinMode(sEcho, INPUT);
    pinMode(swEcho, INPUT);

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

void stop(void){
  for (int i = 255; i >= 0; i-=35) {
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
    nwDistance = (sensors.dist(0) * CMCONVERSION);
    nDistance = (sensors.dist(1) * CMCONVERSION);
    neDistance = (sensors.dist(2) * CMCONVERSION);
    seDistance = (sensors.dist(3) * CMCONVERSION);
    sDistance = (sensors.dist(4) * CMCONVERSION);
    swDistance = (sensors.dist(5) * CMCONVERSION);

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
            delay(60);
        }else{
            //Power for the motors is the y coordinate minus the modifier if the user wants to turn
            //If the joystick is at (255,255) in theory the right motor will be powered to 0 and the left to 255
            analogWrite(enA, abs(ycoordinate) - abs(rightTurn));
            analogWrite(enB, abs(ycoordinate) - abs(leftTurn));
            digitalWrite(in1, LOW);
            digitalWrite(in2, HIGH);
            digitalWrite(in3, LOW);
            digitalWrite(in4, HIGH);
            delay(60);
        } 
    } else if (ycoordinate > 15){
        //Checking if the front sensors are blocked while driving forward
        if (nwDistance < 12 || nDistance < 12 || neDistance < 12){
            stop();
            delay(60);
        }else{
            //Same thing here as before
            analogWrite(enA, abs(ycoordinate) - abs(rightTurn));
            analogWrite(enB, abs(ycoordinate) - abs(leftTurn));
            digitalWrite(in1, HIGH);
            digitalWrite(in2, LOW);
            digitalWrite(in3, HIGH);
            digitalWrite(in4, LOW);
            delay(60); 
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

void loop()
{
    for (int i = 0; i < 6; i++ ){
        run();
        /*
        Serial.print("Pin");
        Serial.print(i); 
        Serial.print(" = ");
        Serial.print(sensors.dist(i));
        Serial.print("\n"); //return curent distance (cm) in serial for sensor 1 to 6
        delay(1000);
        */
        }                       // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.
}