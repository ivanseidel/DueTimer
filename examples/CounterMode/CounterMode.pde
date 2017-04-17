/*
  Arduino Due Timer in Counter mode example
  This example code is in the public domain
 */

#include <TFT.h>  // Arduino LCD library
#include <SPI.h>
#include <DueTimer.h>

// pin definition for the Due
#define cs   7
#define dc   3
#define rst  6

// create an instance of the library
TFT TFTscreen = TFT(cs, dc, rst);

long tickms = 0;
long counterValue = 0;
byte counterChange = 0;

char charBuffer[20];

void timerISR() {
  // 1ms tick
  tickms++;
}

void timerGate() {
  // 1 second gate signal
  // Get counter value and reset
  counterValue = Timer3.counterValueAndReset();
  counterChange++;
}

void setup() {
  // buil in led
  pinMode(LED_BUILTIN, OUTPUT);

  // Put this line at the beginning of every sketch that uses the GLCD:
  TFTscreen.begin();

  // clear the screen with a black background
  TFTscreen.background(0, 0, 0);

  // write the static text to the screen
  // set the font color to white
  TFTscreen.stroke(255, 255, 255);
  // set the font size
  TFTscreen.setTextSize(2);
  // write the text to the top left corner of the screen
  TFTscreen.text("Count Value :\n ", 0, 0);
  // ste the font size very large for the loop
  TFTscreen.setTextSize(3);

  TFTscreen.fill(0, 0, 0);
  TFTscreen.noStroke();

  // for debugging monitor
  //Serial.begin(9600); //This pipes to the serial monitor
  //Serial1.begin(9600); //This is the UART, pipes to sensors attached to board

  // Timer 1 TIOB output (analog6) is wired on timer 3 input clock TCLK3 (Analog3)
  
  // Start timer 1, 100 Hz generator (Analog6)
  Timer1.attachInterrupt(timerISR).setPeriodMilliSeconds(10).setTimeOnBMilliSeconds(1).enablePinB().start();
  // Start timer 2, 1s gate signal
  Timer2.attachInterrupt(timerGate).setPeriodMilliSeconds(1000).start();
  // Start timer 3, counter mode on TCLK3 (Analog3)
  Timer3.setCounter().start();
}

void loop() {
  
  if (counterChange) {
    TFTscreen.rect(0,20,TFTscreen.width(),40);
    TFTscreen.stroke(255, 255, 255);

    sprintf(charBuffer,"%u",counterValue);
    TFTscreen.text(charBuffer, 0, 20);
    
    TFTscreen.noStroke();
    counterChange--;
  }
 
}


