/*
  BaseTimer.h - Basic Timer interface
  For instructions, go to https://github.com/ivanseidel/????

  Created by Ivan Seidel Gomes, October, 2013.
  Released into the public domain.
*/

#ifndef BaseTimer_h
#define BaseTimer_h

class BaseTimer{
protected:
	virtual BaseTimer attachInterrupt(void (*isr)());
	virtual BaseTimer detachInterrupt();

	virtual BaseTimer start(long microseconds = -1);
	virtual BaseTimer stop();

	virtual BaseTimer setFrequency(double frequency);
	virtual double getFrequency();

	virtual BaseTimer setPeriod(long microseconds);
	virtual long getPeriod();
};

#endif