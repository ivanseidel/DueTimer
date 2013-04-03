#include <DueTimer.h>

void firstHandler(){
	Serial.println("[-  ] First Handler!");
}

void secondHandler(){
	Serial.println("[ - ] Second Handler!");
}

void thirdHandler(){
	Serial.println("[  -] Third Handler!");
}

void setup(){
	Serial.begin(9600);

	Timer3.attachInterrupt(firstHandler).start(500000); // Every 500ms
	Timer4.attachInterrupt(secondHandler).setFrequency(1).start();
	Timer5.attachInterrupt(thirdHandler).setFrequency(10);
}

void loop(){
	delay(2000);
	Timer5.start();

	delay(2000);
	Timer5.stop();
}