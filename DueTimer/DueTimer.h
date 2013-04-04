/*
  DueTimer.h - DueTimer header file, definition of methods and attributes...
  For instructions, go to https://github.com/ivanseidel/DueTimer

  Created by Ivan Seidel Gomes, March, 2013.
  Released into the public domain.
*/

#ifdef __arm__

#ifndef DueTimer_h
#define DueTimer_h

#include "Arduino.h"

#include <inttypes.h>

class DueTimer
{
protected:
	int timer; // Stores the object timer id (to access Timer struct array)
	int frequency[9]; // Stores the object timer frequency (to access know current timer period, frequency...)

	void savePeriod(long period); // Saves the period

	DueTimer uint8_t pickClock(uint32_t frequency, uint32_t& retRC);


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

	long getFrequency();
	long getPeriod();
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
#endif
