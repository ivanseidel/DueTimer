/*
	ArduinoTimer.h - ArduinoTimer header file, definition of methods and attributes
	For instructions, go to https://github.com/ivanseidel/ArduinoTimer

	Created by Ivan Seidel Gomes, October, 2013.
	Modified by Philipp Klaus, June 2013.
	Released into the public domain.
*/

#ifndef ArduinoTimer_h
#define ArduinoTimer_h

#include <inttypes.h>

/*
	Includes specific archtecture timer definitions
*/
#include <ArchTimer.h>

class ArduinoTimer: public BaseTimer
{
protected:
	int timerId;

public:

	static virtual ArduinoTimer getAvailable();

	ArduinoTimer(int _timerId);

	virtual ArduinoTimer attachInterrupt(void (*isr)());
	virtual ArduinoTimer detachInterrupt();

	virtual ArduinoTimer start(long microseconds = -1);
	virtual ArduinoTimer stop();

	virtual ArduinoTimer setFrequency(double frequency);
	virtual double getFrequency();

	virtual ArduinoTimer setPeriod(long microseconds);
	virtual long getPeriod();
};

#endif
