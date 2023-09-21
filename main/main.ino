//This file is to test is using one common trigger pin for all 6 sensors

#include <Arduino.h>
#include <HCSR04.h>
#include <stdint.h>
#include <math.h>

#define CMCONVERSION 0.393701

//#define trig 2
// Trigger pins for each bank of sensors
#define trign 2
#define trigs 3
//Northwest Sensor
#define nwEcho 5
//North Sensor
#define nEcho 6
//Northeast Sensor
#define neEcho 7
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

// Neutral stick values so that the joystick inputs can be read properly, allows for neg and pos position values for both 
// coordinates
#define xneutral 503
#define yneutral 511

//The main difference of the branches is here. By using a different class constructor, we can use one common trigger pin
//for each bank of sensors rather than two dedicated pins for every sensor. 12 pins becomes 8, making the board design simpler

//HCSR04 sensors(trig, (int[6]) {nwEcho, nEcho, neEcho, seEcho, sEcho, seEcho}, 6);
HCSR04 north(trign, (int[3]) {nwEcho, nEcho, neEcho}, 3);
HCSR04 south(trigs, (int[3]) {swEcho, sEcho, seEcho}, 3);

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

void stop(int power){
    if (power < 0){
        for (int i = power; i >= 0; i-=35) {
	        analogWrite(enA, i);
	        analogWrite(enB, i);
		    delay(20);
	    } 
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
    nwDistance = (north.dist(0) * CMCONVERSION);
    nDistance = (north.dist(1) * CMCONVERSION);
    neDistance = (north.dist(2) * CMCONVERSION);
    swDistance = (south.dist(0) * CMCONVERSION);
    sDistance = (south.dist(1) * CMCONVERSION);
    seDistance = (south.dist(2) * CMCONVERSION);

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
    int16_t ycoordinate = (yval - yneutral) * joystickRatio;
    int16_t xcoordinate = (xval - xneutral) * joystickRatio;

    //Checking the x coordinate from the joystick to see if hte user wants to turn
    if (xcoordinate < -20){
        leftTurn = 0;
        rightTurn = abs(xcoordinate) - 25;
    }
    if (xcoordinate > 20){
        leftTurn = abs(xcoordinate) - 10;
        rightTurn = 0;
    }
    int powerRight = 0;
    int powerLeft = 0;

    if (ycoordinate < -15){
        //Checking if the back sensors are blocked while going backwards
        if (swDistance < 12 || sDistance < 12 || seDistance < 12){
            stop(powerLeft);
            delay(60);
        }else{
            //Power for the motors is the y coordinate minus the modifier if the user wants to turn
            //If the joystick is at (255,255) in theory the right motor will be powered to 0 and the left to 255
            powerRight = abs(ycoordinate) - abs(rightTurn);
            powerLeft = abs(ycoordinate) - abs(leftTurn);
            analogWrite(enA, powerRight);
            analogWrite(enB, powerLeft);
            digitalWrite(in1, LOW);
            digitalWrite(in2, HIGH);
            digitalWrite(in3, LOW);
            digitalWrite(in4, HIGH);
            delay(60);
        } 
    } else if (ycoordinate > 15){
        //Checking if the front sensors are blocked while driving forward
        if (nwDistance < 12 || nDistance < 12 || neDistance < 12){
            stop(powerLeft);
            delay(60);
        }else{
            //Same thing here as before
            powerRight = abs(ycoordinate) - abs(rightTurn);
            powerLeft = abs(ycoordinate) - abs(leftTurn);
            analogWrite(enA, powerRight);
            analogWrite(enB, powerLeft);
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
        }
}