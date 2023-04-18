/*
MIT License

Copyright (c) 2017 gamegine

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "HCSR04.h"
////////////////////////////////////consttruct/destruct
void HCSR04::init(int out, int echo[], int n)
{
	this->out = out;
	this->echo = echo;
	this->n = n;
	pinMode(this->out, OUTPUT);
	for (int i = 0; i < n; i++)
		pinMode(this->echo[i], INPUT);
}
HCSR04::HCSR04(int out, int echo) { this->init(out, new int[1]{echo}, 1); }
HCSR04::HCSR04(int out, int echo[], int n) { this->init(out, echo, n); }
HCSR04::~HCSR04()
{
	delete[] this->echo;
}

///////////////////////////////////////////////////dist
float HCSR04::dist(int n) const
{
	digitalWrite(this->out, LOW);
	delayMicroseconds(2);
	digitalWrite(this->out, HIGH);
	delayMicroseconds(10);
	digitalWrite(this->out, LOW);
	noInterrupts();
	float d = pulseIn(this->echo[n], HIGH, 23529.4); // max sensor dist ~4m
	interrupts();
	return d / 58.8235;
}
float HCSR04::dist() const { return this->dist(0); }