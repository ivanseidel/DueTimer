/*
  DueTimer.h - DueTimer header file, definition of methods and attributes...
  For instructions, go to https://github.com/ivanseidel/DueTimer

  Created by Ivan Seidel Gomes, March, 2013.
  Modified by Philipp Klaus, June 2013.
  Released into the public domain.
*/

#include "Arduino.h"

#if defined(_SAM3XA_)

#ifndef DueTimer_h
#define DueTimer_h

#include <inttypes.h>

/*
	This fixes compatibility for Arduono Servo Library.
	Uncomment to make it compatible.

	Note that:
		+ Timers: 0,2,3,4,5 WILL NOT WORK, and will
				  neither be accessible by Timer0,...
*/
// #define USING_SERVO_LIB	true

#ifdef USING_SERVO_LIB
	#warning "HEY! You have set flag USING_SERVO_LIB. Timer0, 2,3,4 and 5 are not available"
#endif


#if defined TC2
#define NUM_TIMERS  9
#else
#define NUM_TIMERS  6
#endif

class DueTimer
{
protected:

	// Represents the timer id (index for the array of Timer structs)
	const unsigned short timer;

	int _running = 0;

	// Stores the object timer frequency
	// (allows to access current timer period and frequency):
	static double _frequency[NUM_TIMERS];

	// Picks the best clock to lower the error
	static uint8_t bestClock(double frequency, uint32_t& retRC);

  // Make Interrupt handlers friends, so they can use callbacks
  friend void TC0_Handler(void);
  friend void TC1_Handler(void);
  friend void TC2_Handler(void);
  friend void TC3_Handler(void);
  friend void TC4_Handler(void);
  friend void TC5_Handler(void);
#if NUM_TIMERS > 6
  friend void TC6_Handler(void);
  friend void TC7_Handler(void);
  friend void TC8_Handler(void);
#endif

	static void (*callbacks[NUM_TIMERS])();

	struct Timer
	{
		Tc *tc;
		uint32_t channel;
		IRQn_Type irq;
	};

	// Store timer configuration (static, as it's fixed for every object)
	static const Timer Timers[NUM_TIMERS];

public:

	static DueTimer getAvailable(void);

	DueTimer(unsigned short _timer);
	DueTimer& attachInterrupt(void (*isr)());
	DueTimer& detachInterrupt(void);
	DueTimer& start(double microseconds = -1);
	DueTimer& stop(void);
	DueTimer& setFrequency(double frequency);
	DueTimer& setPeriod(double microseconds);

	double getFrequency(void) const;
	double getPeriod(void) const;

  inline __attribute__((always_inline)) bool operator== (const DueTimer& rhs) const
    {return timer == rhs.timer; };
  inline __attribute__((always_inline)) bool operator!= (const DueTimer& rhs) const
    {return timer != rhs.timer; };
};

// Just to call Timer.getAvailable instead of Timer::getAvailable() :
extern DueTimer Timer;

extern DueTimer Timer1;
// Fix for compatibility with Servo library
#ifndef USING_SERVO_LIB
	extern DueTimer Timer0;
	extern DueTimer Timer2;
	extern DueTimer Timer3;
	extern DueTimer Timer4;
	extern DueTimer Timer5;
#endif
#if NUM_TIMERS > 6
extern DueTimer Timer6;
extern DueTimer Timer7;
extern DueTimer Timer8;
#endif

#endif

#else
	#error Oops! Trying to include DueTimer on another device?
#endif
