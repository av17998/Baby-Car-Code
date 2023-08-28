//This file is to test is using one common trigger pin for all 6 sensors

#include <Arduino.h>
#include <HCSR04.h>

HCSR04 hc(2, int[2]{5, 6}, 2); //initialisation class HCSR04 (trig pin , echo pin, number of sensor)

void setup()
{ Serial.begin(9600); }

void loop()
{
    for (int i = 0; i < 2; i++ )
        Serial.println( hc.dist(i) ); //return curent distance (cm) in serial for sensor 1 to 6
    delay(60);                        // we suggest to use over 60ms measurement cycle, in order to prevent trigger signal to the echo signal.
}