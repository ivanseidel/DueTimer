/*
  DueTimer.h - DueTimer header file, definition of methods and attributes...
  For instructions, go to https://github.com/ivanseidel/DueTimer

  Created by Ivan Seidel Gomes, March, 2013.
  Released into the public domain.
*/

#ifndef DueTimer_h
#define DueTimer_h

#include "Arduino.h"

#include <inttypes.h>

class DueTimer
{
protected:
	int timer; // Stores the object timer id (to access Timer struct array)

public:
	struct Timer
	{
		Tc *tc;
		uint32_t channel;
		IRQn_Type irq;
	};

	static const Timer Timers[9];
	static void (*callbacks[9])();

	DueTimer(int _timer);
	DueTimer attachInterrupt(void (*isr)());
	DueTimer start(long microseconds = -1);
	DueTimer stop();
	DueTimer setFrequency(long frequency);
	DueTimer setPeriod(long microseconds);
};

extern DueTimer Timer0;
extern DueTimer Timer1;
extern DueTimer Timer2;
extern DueTimer Timer3;
extern DueTimer Timer4;
extern DueTimer Timer5;
extern DueTimer Timer6;
extern DueTimer Timer7;
extern DueTimer Timer8;

#endif