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
	static int _frequency[9]; // Stores the object timer frequency (to access know current timer period, frequency...)

	static uint8_t bestClock(uint32_t frequency, uint32_t& retRC); // Picks the best clock to lower the error

public:
	struct Timer
	{
		Tc *tc;
		uint32_t channel;
		IRQn_Type irq;
	};

	static DueTimer getAvaliable();

	static const Timer Timers[9]; // Store timer configuration (static, as it's fix for every object)
	static void (*callbacks[9])(); // Needs to be public, because the handlers are outside class

	DueTimer(int _timer);
	DueTimer attachInterrupt(void (*isr)());
	DueTimer detachInterrupt();
	DueTimer start(long microseconds = -1);
	DueTimer stop();
	DueTimer setFrequency(long frequency);
	DueTimer setPeriod(long microseconds);


	long getFrequency();
	long getPeriod();
};

extern DueTimer Timer; // Just to call Timer.getAvaliable instead of Timer::getAvaliable()

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

#else
	#pragma message("Ops! Trying to include DueTimer on another device?")
#endif
