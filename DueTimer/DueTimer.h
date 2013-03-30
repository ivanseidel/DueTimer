#ifndef DueTimer_h
#define DueTimer_h

#include "Arduino.h"

#include <stdlib.h>
#include <inttypes.h>

class DueTimer
{
public:
	struct Timer
	{
		Tc *tc;
		uint32_t channel;
		IRQn_Type irq;
	};

	static const Timer Timers[6];
	static void (*callbacks[6])();
	int timer; // Stores the object timer id (to acces Timers struct array)

	DueTimer(int _timer);
	DueTimer attachInterrupt(void (*isr)());
	DueTimer start(long frequency = -1);
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

#endif