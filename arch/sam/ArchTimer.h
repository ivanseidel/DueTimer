/*
	ArchTimer.h - SAM Only definitions
	For instructions, go to https://github.com/ivanseidel/ArduinoTimer

	Created by Ivan Seidel Gomes, October, 2013.
	Released into the public domain.
*/

/**
 * SAM Only definitions
 * --------------------
 */

/*
	Frequencyes for the timers are saved here
*/
double ArduinoTimer_frequency[9];

/*
	Picks the best clock to lower the error
*/
uint8_t ArduinoTimer_bestClock(double frequency, uint32_t& retRC);

/*
	Represents a SAM Timer configuration
*/
struct ArduinoTimer_Timer_t
{
	Tc *tc;
	uint32_t channel;
	IRQn_Type irq;
};

// Store timer configuration (static, as it's fix for every object)
const Timer ArduinoTimer_Timers[9];

// Needs to be public, because the handlers are outside class:
void (*ArduinoTimer_callbacks[9])();
