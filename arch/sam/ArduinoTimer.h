/*
	ArduinoTimer.h - ArduinoTimer header file, definition of methods and attributes...
	For instructions, go to https://github.com/ivanseidel/ArduinoTimer

	Created by Ivan Seidel Gomes, October, 2013.
	Modified by Philipp Klaus, June 2013.
	Released into the public domain.
*/

#ifndef ArduinoTimer_h
#define ArduinoTimer_h

#include "Arduino.h"

#include <inttypes.h>

class ArduinoTimer
{
protected:
	int timerId;

	// Stores the object timer frequency
	// (allows to access current timer period and frequency):
	static double _frequency[9];

	// Picks the best clock to lower the error
	static uint8_t bestClock(double frequency, uint32_t& retRC);

public:
	struct Timer
	{
		Tc *tc;
		uint32_t channel;
		IRQn_Type irq;
	};

	static ArduinoTimer getAvailable();

	// Store timer configuration (static, as it's fix for every object)
	static const Timer Timers[9];

	// Needs to be public, because the handlers are outside class:
	static void (*callbacks[9])();

	ArduinoTimer(int _timerId);

	virtual BaseTimer attachInterrupt(void (*isr)());
	virtual BaseTimer detachInterrupt();

	virtual BaseTimer start(long microseconds = -1);
	virtual BaseTimer stop();

	virtual BaseTimer setFrequency(double frequency);
	virtual double getFrequency();

	virtual BaseTimer setPeriod(long microseconds);
	virtual long getPeriod();
};

// Just to call Timer.getAvailable instead of Timer::getAvailable() :
extern ArduinoTimer Timer;

extern ArduinoTimer Timer0;
extern ArduinoTimer Timer1;
extern ArduinoTimer Timer2;
extern ArduinoTimer Timer3;
extern ArduinoTimer Timer4;
extern ArduinoTimer Timer5;
extern ArduinoTimer Timer6;
extern ArduinoTimer Timer7;
extern ArduinoTimer Timer8;

#endif

#else
	#pragma message("Ops! Trying to include ArduinoTimer on another device?")
#endif
