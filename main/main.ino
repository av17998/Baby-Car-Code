//This file is to test is using one common trigger pin for all 6 sensors

#include <Arduino.h>
#include <HCSR04.h>
int testpins[6] = {5, 6, 7, 8, 9, 10};
HCSR04 hc(2, testpins, 6); //initialisation class HCSR04 (trig pin , echo pin, number of sensor)

void setup()
{ Serial.begin(9600); }

void loop()
{
    for (int i = 0; i < 6; i++ ){
        Serial.print("Pin");
        Serial.print(i); 
        Serial.print(" = ");
        Serial.print(hc.dist(i));
        Serial.print("\n"); //return curent distance (cm) in serial for sensor 1 to 6
        delay(1000);
        }                       // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.
}