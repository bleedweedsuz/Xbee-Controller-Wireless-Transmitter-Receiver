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
//INCLUDE FILE
#include <SoftwareSerial.h>
//Decelerations of all variables
//Analog Pin
int AL1=A0,AR1=A1,AR2=A2;//Flight Controls
//Digital Pin 
int L2=7,R2=6;//Flight Controls
int RGBLed=8,YellowLed=9;//Data Signal LED
//Serial Communication
SoftwareSerial mySerial(10, 11); // RX, TX
int rolllMode=0;//0=off,1=L-off,2=R-off
int pitchValue=90;
int yawValue=90;
int thrustValue=0;
//Sampling Count variables
//Yaw
int yawSampleCount=0;
int yawSamplesTemp[5];
//pitch
int pitchSampleCount=0;
int pitchSamplesTemp[5];
//thrust
int thrustSampleCount=0;
int thrustSamplesTemp[5];
//MIN MAX DATA
int MIN=0,MAX=200;
//control state change

int rState= 0,lState = 0;
int rOldState = 0,lOldState = 0;
void setup()
{
	//Setup the Serial Communication
	Serial.begin(9600);
	//Setup the Software Serial Communication
	mySerial.begin(9600);
	//Setup for digital input
	pinMode(L2,INPUT);
	pinMode(R2,INPUT);
	//Setup the Data Signal Led
	pinMode(RGBLed,OUTPUT);
	pinMode(YellowLed,OUTPUT);
	//Signal Blink for device
	dataTransmit(HIGH);
	pinActiveState(HIGH);
	delay(2000);
	dataTransmit(LOW);
	pinActiveState(LOW);
}
void loop()
{
	//Button Controls
	ButtonControls();
	//Roll Buttons
	//rollButtons();
	//Yaw Buttons
	analogYaw();
	//Pitch Buttons
	analogPitch();
	//Thrust Buttons
	analogThrust();
	//turn off the data transmit signal
	pinActiveState(0);
}
void ButtonControls(){
	int rState = digitalRead(R2);
	int lState = digitalRead(L2);
	if(lState == 0 && lOldState == 1){
		sendSignal("A00");//ARMING SIGNAL
		dataTransmit(HIGH);
		delay(10000);//wait for 10 sec
		dataTransmit(LOW);
	}
	if(rState == 0 && rOldState == 1){
		sendSignal("D10");//DISARMING SIGNAL
		dataTransmit(HIGH);
		delay(20000);//wait for 10 sec
		dataTransmit(LOW);
	}
	rOldState =rState;
	lOldState =lState;
}
void rollButtons(){
	int L2State,R2State;
	L2State = digitalRead(L2);
	R2State = digitalRead(R2);
	//check for l2 first and used l2 if activate
	if(L2State==1){
		pinActiveState(1);
		if(rolllMode !=1){
			//Transmit the signal GO LEFT
			rolllMode=1;
			sendSignal("R1");
		}
	}
	else if(R2State==1){
		pinActiveState(1);
		if(rolllMode !=2){
			//Transmit the signal GO RIGHT
			rolllMode=1;
			sendSignal("R2");
		}
	}
	else{
		pinActiveState(0);
		if(rolllMode ==1 || rolllMode ==2){
			//Transmit the signal GO Stop Roll
			rolllMode=0;
			sendSignal("R0");
		}
	}
}
void analogYaw(){
	int noOfSample =5;
	int yawTempVal = analogRead(AR1);
	int yawMapVal = map(yawTempVal,0,1024,MIN,MAX);
	if(yawSampleCount < noOfSample){
		//sampling process
		yawSamplesTemp[yawSampleCount]=yawMapVal;
		yawSampleCount++;
	}
	//set the mean sample
	if(yawSampleCount >= noOfSample){
		//Digital Sampling process
		int i;
		int totalSum=0;
		for(i=0;i<noOfSample;i++){
			totalSum +=yawSamplesTemp[i];
		}
		int yawMeanval = totalSum /noOfSample;
		if(yawMeanval !=yawValue){
			yawValue =yawMeanval;
			//packing buffer data
			//convert integer to char array
			char valBuffer[3];
			itoa(yawValue,valBuffer,10);
			char data[4];
			strcpy(data,"Y");
			strcat(data,valBuffer);
			//send data packet signal
			sendSignal(data);
		}
		yawSampleCount=0;	
		delay(10);
	}
}
void analogPitch(){
	int noOfSample =5;
	int pitchTempVal = analogRead(AR2);
	int pitchMapVal = map(pitchTempVal,0,1024,MAX,MIN);
	if(pitchSampleCount < noOfSample){
		//sampling process
		pitchSamplesTemp[pitchSampleCount]=pitchMapVal;
		pitchSampleCount++;
	}
	//set the mean sample
	if(pitchSampleCount >= noOfSample){
		//Digital Sampling process
		int i;
		int totalSum=0;
		for(i=0;i<noOfSample;i++){
			totalSum +=pitchSamplesTemp[i];
		}
		int pitchMeanval = totalSum /noOfSample;
		if(pitchMeanval !=pitchValue){
			pitchValue =pitchMeanval;
			//packing buffer data
			//convert integer to char array
			char valBuffer[3];
			itoa(pitchValue,valBuffer,10);
			char data[4];
			strcpy(data,"P");
			strcat(data,valBuffer);
			//send data packet
			//Serial.println(data);
			//send data packet signal
			sendSignal(data);
		}
		pitchSampleCount=0;
		delay(10);
	}	
}
void analogThrust(){
	int noOfSample =5;
	int thrustTempVal = analogRead(AL1);
	int thrustMapVal = map(thrustTempVal,0,1024,210,100);
	if(thrustSampleCount < noOfSample){
		//sampling process
		thrustSamplesTemp[thrustSampleCount]=thrustMapVal;
		thrustSampleCount++;
	}
	//set the mean sample
	if(thrustSampleCount >= noOfSample){
		//Digital Sampling process
		int i;
		int totalSum=0;
		for(i=0;i<noOfSample;i++){
			totalSum +=thrustSamplesTemp[i];
		}
		int thrustMeanval = totalSum /noOfSample;
		if(thrustMeanval !=thrustValue){
			thrustValue =thrustMeanval;
			//packing buffer data
			//convert integer to char array
			char valBuffer[3];
			itoa(thrustValue,valBuffer,10);
			char data[4];
			strcpy(data,"T");
			strcat(data,valBuffer);
			//send data packet
			//Serial.println(data);
			//send data packet signal
			sendSignal(data);
		}
		thrustSampleCount=0;
		delay(10);
	}
	
}
void sendSignal(char data[]){
	//strcat(data,"|");
	mySerial.write(data);
	Serial.println(data);
	//send Signal
	pinActiveState(1);
	delay(10);
}
//-------------------------->
void dataTransmit(int state){
	digitalWrite(RGBLed,state);
}
void pinActiveState(int state){
	digitalWrite(YellowLed,state);
}