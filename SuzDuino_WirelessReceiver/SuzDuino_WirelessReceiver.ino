/*	Written by Sujan Thapa
	Date:2015-07-11
	
	This project is made for flight control transmitter. This project used suzduino board
	as hardware. This project also used USB JoyStick, I only hacked the circuit so the controller transmit Analog as well as Digital signal.
	This project is made for education purpose, freely available those who want a simple transmitter.
	
	Board power supply:+5v
	Analog Signal upto 5V; Total used Analog:3
	Digital Signal 5V;Total Used Digital Signal:2
	--------------------------------------------------------------------
	Total used signal: 5
	--------------------------------------------------------------------	
	The MIT License (MIT)
	Copyright (c) 2015 Sujan Thapa

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sub license, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.

	http://opensource.org/licenses/mit-license.php
	--------------------------------------------------------------------
*/
#include <string.h>
#include <Servo.h>
#include <SoftwareSerial.h>
Servo Throttle;
Servo Rudder;
Servo Aileron;
Servo Elevator;
Servo Auxiliary;
int iPin=8;
SoftwareSerial mySerial(10, 11); // RX, TX
bool isLogger =true;
void setup()
{
	//SETUP FOR RECEIVER//
	Rudder.attach(5);
	Aileron.attach(6);
	Auxiliary.attach(7);
	Throttle.attach(9);
	Elevator.attach(12);
	pinMode(iPin,OUTPUT);
	Serial.begin(9600);
	mySerial.begin(9600);
	//------------------//
	LedPattern(2,true);
	Serial.println("Flight Module Initialize");
}
void loop(){
	if(mySerial.available() > 0){
		String bufferData="";
		while(mySerial.available()){
			char val = mySerial.read();
			bufferData +=String(val);
		}
		int lenData = bufferData.length();
		StringExploder(bufferData);
		if(isLogger){
			Serial.print(">>");
			Serial.println(bufferData);
		}
	}
}
void StringExploder(String data){
	char PIN = data[0];
	int val = data.substring(1,data.length()).toInt() * 10;
	switch(PIN){
		case 'A':{
			LedPattern(3,false);
			CalibrateAndArm();
			LedPattern(3,true);
		}break;
		case 'D':{
			LedPattern(4,false);
			Disarmed();
			LedPattern(4,true);
		}break;
		case 'T':{
			ThrottlePower(val);
		}break;
		case 'P':{
			ElevatorPower(val);
		}break;
		case 'Y':{
			RudderPower(val);
		}break;
		
		default:break;
	}	
}
void ThrottlePower(int val){
	Throttle.writeMicroseconds(val);
}
void RudderPower(int val){
	Rudder.writeMicroseconds(val);
}
void ElevatorPower(int val){
	Elevator.writeMicroseconds(val);
}
void CalibrateAndArm(){
	ledIndicator(true);
	Serial.println("Calibrating..");
	Aileron.writeMicroseconds(1500);
	Elevator.writeMicroseconds(1500);
	Throttle.writeMicroseconds(2000);
	Rudder.writeMicroseconds(1500);
	delay(2000);
	Aileron.writeMicroseconds(1500);
	Elevator.writeMicroseconds(1500);
	Throttle.writeMicroseconds(1000);
	Rudder.writeMicroseconds(1500);
	Serial.println("Calibrated");
	delay(500);
	//--------------------------//
	Serial.println("Arming..");
	Throttle.writeMicroseconds(1000);
	Rudder.writeMicroseconds(1500);
	Aileron.writeMicroseconds(1500);
	Elevator.writeMicroseconds(1500);
	Auxiliary.writeMicroseconds(1000);
	delay(4000);
	Rudder.writeMicroseconds(2000);
	delay(1000);
	Rudder.writeMicroseconds(1500);
	Serial.println("Armed..");
}
void Disarmed(){
	Serial.println("DisArming..");
	Throttle.writeMicroseconds(1000);
	Rudder.writeMicroseconds(1500);
	Aileron.writeMicroseconds(1500);
	Elevator.writeMicroseconds(1500);
	Auxiliary.writeMicroseconds(1000);
	delay(4000);
	Rudder.writeMicroseconds(1000);
	delay(1000);
	Rudder.writeMicroseconds(1500);
	Serial.println("DisArmed..");
}
void ledIndicator(int state){
	digitalWrite(iPin,state);
}
void LedPattern(int val,bool isFast){
	//Pattern Maker
	/*Blink Pattern Understanding
	2 = start up; //Fast
	3 = calibrating and arming begin//Slow
	3 = calibrating and arming finished//Fast
	4 = disarming begin//Slow
	4 = disarming finished//fast
	*/
	int delayTime;
	if(isFast){
		delayTime =100;
	}
	else{
		delayTime = 200;
	}
	for(int i=0;i < val;i++){
		ledIndicator(1);
		delay(delayTime);
		ledIndicator(0);
		delay(delayTime);
	}
}