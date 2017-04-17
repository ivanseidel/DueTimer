/*
  DueTimer.h - DueTimer header file, definition of methods and attributes...
  For instructions, go to https://github.com/ivanseidel/DueTimer

  Created by Ivan Seidel Gomes, March, 2013.
  Modified by Philipp Klaus, June 2013.
  Released into the public domain.
*/

#ifdef __arm__

#ifndef DueTimer_h
#define DueTimer_h

#include "Arduino.h"

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


#define NUM_TIMERS  9

class DueTimer
{
protected:

	// Represents the timer id (index for the array of Timer structs)
	const unsigned short timer;

	// Stores the object timer frequency
	// (allows to access current timer period and frequency):
	static double _frequency[NUM_TIMERS];
	static uint32_t _statusRegister[NUM_TIMERS];

	// Picks the best clock to lower the error
	static uint8_t bestClock(double frequency, uint32_t& retRC);
	static uint8_t bestCaptureClock(double maxPeriodMicroSeconds, double& resolution, uint32_t& maxPeriodRC);
	
	uint32_t getDutyCycleRX(double percent);

  // Make Interrupt handlers friends, so they can use callbacks
  friend void TC0_Handler(void);
  friend void TC1_Handler(void);
  friend void TC2_Handler(void);
  friend void TC3_Handler(void);
  friend void TC4_Handler(void);
  friend void TC5_Handler(void);
  friend void TC6_Handler(void);
  friend void TC7_Handler(void);
  friend void TC8_Handler(void);

	static void (*callbacks[NUM_TIMERS])();

	struct Timer
	{
		Tc *tc;
		uint32_t channel;
		IRQn_Type irq;
		// TIOA
		Pio *pioa;
		uint32_t tioaBits;
		EPioType tioaPeriph;
		// TIOB
		Pio *piob;
		uint32_t tiobBits;
		EPioType tiobPeriph;
		// TCLK
		Pio *pioclk;
		uint32_t tioClkBits;
		EPioType tioClkPeriph;
	};

	// Store timer configuration (static, as it's fixed for every object)
	static const Timer Timers[NUM_TIMERS];

public:
	static DueTimer getAvailable(void);

	DueTimer(unsigned short _timer);
	DueTimer& attachInterrupt(void (*isr)());
	DueTimer& detachInterrupt(void);
	DueTimer& start(void);
	DueTimer& start(double microseconds);
	DueTimer& stop(void);
	DueTimer& setFrequency(double frequency);
	double getFrequency(void) const;
	double getPeriod(void) const;
	
	DueTimer& setPeriod(double microseconds);
	DueTimer& setPeriodMilliSeconds(double milliseconds) {
		return setPeriod(milliseconds * 1.0e3);
	};
	
	DueTimer& setDutyCycleA(double percent);
	DueTimer& setTimeOnA(double microseconds) {
		return setDutyCycleA( microseconds / getPeriod());
	}
	DueTimer& setTimeOnAMilliSeconds(double milliseconds) {
		return setTimeOnA(milliseconds * 1.0e3);
	};

	DueTimer& setDutyCycleB(double percent);
	DueTimer& setTimeOnB(double microseconds) {
		return setDutyCycleB( microseconds / getPeriod());
	};
	DueTimer& setTimeOnBMilliSeconds(double milliseconds) {
		return setTimeOnB(milliseconds * 1.0e3);
	};
	
	uint32_t statusRegister(void);
	DueTimer& enablePinB();
	DueTimer& enablePinA();
	DueTimer& enablePinClock();

	// Capture mode, triggered by TIOA pin
	DueTimer& setCapture(double maxPeriodMicroSeconds);
	uint32_t captureValueA(void);
	uint32_t captureValueB(void);
	DueTimer& resetCounterValue(void);
	double valueToMicroSeconds(uint32_t value);
	double valueToMilliSeconds(uint32_t value);
	double valueToSeconds(uint32_t value);
	inline double getResolutionMicroSeconds(void) const;

	DueTimer& setCaptureMilliSeconds(double maxPeriodMilliSeconds) {
		return setCapture(maxPeriodMilliSeconds * 1.0e3);
	};
	DueTimer& setCaptureSeconds(double maxPeriodSeconds) {
		return setCapture(maxPeriodSeconds * 1.0e6);
	};

	// Counter mode, external clock
	DueTimer& setCounter(void);
	uint32_t counterValue(void);
	uint32_t counterValueAndReset(void);
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
extern DueTimer Timer6;
extern DueTimer Timer7;
extern DueTimer Timer8;

#endif

#else
	#error Oops! Trying to include DueTimer on another device?
#endif
