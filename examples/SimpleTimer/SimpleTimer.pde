#include <DueTimer.h>

int myLed = 13;

bool ledOn = false;
void myHandler(){
	ledOn = !ledOn;

	digitalWrite(myLed, ledOn); // Led on, off, on, off...
}

void setup(){
	pinMode(myLed, OUTPUT);

	Timer3.attachInterrupt(myHandler);
	Timer3.start(50000); // Calls every 50ms
}

void loop(){

	while(1){
		// I'm stuck in here! help me...
	}
	
}