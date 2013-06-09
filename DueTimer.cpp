/*
  DueTimer.cpp - Implementation of Timers defined on DueTimer.h
  For instructions, go to https://github.com/ivanseidel/DueTimer

  Created by Ivan Seidel Gomes, March, 2013.
  Modified by Philipp Klaus, June 2013.
  Thanks to stimmer (from Arduino forum), for coding the "timer soul" (Register stuff)
  Released into the public domain.
*/

#include "DueTimer.h"

const DueTimer::Timer DueTimer::Timers[9] = {
	{TC0, 0, TC0_IRQn,   22, PIO_PERIPH_B},
	{TC0, 1, TC1_IRQn,   59, PIO_PERIPH_A}, // A5
	{TC0, 2, TC2_IRQn,   31, PIO_PERIPH_A},
	{TC1, 0, TC3_IRQn,   57, PIO_PERIPH_B}, // A3
	{TC1, 1, TC4_IRQn,   56, PIO_PERIPH_B}, // A2
	{TC1, 2, TC5_IRQn,   67, PIO_PERIPH_A},
	{TC2, 0, TC6_IRQn, NULL, PIO_PERIPH_B}, // n/a
	{TC2, 1, TC7_IRQn, NULL, PIO_PERIPH_B}, // LED "RX"
	{TC2, 2, TC8_IRQn,   30, PIO_PERIPH_B},
};

void (*DueTimer::callbacks[9])() = {};
double DueTimer::_frequency[9] = {-1,-1,-1,-1,-1,-1,-1,-1,-1};

/*
	Initializing all timers, so you can use them like this: Timer0.start();
*/
DueTimer Timer(0);

DueTimer Timer0(0);
DueTimer Timer1(1);
DueTimer Timer2(2);
DueTimer Timer3(3);
DueTimer Timer4(4);
DueTimer Timer5(5);
DueTimer Timer6(6);
DueTimer Timer7(7);
DueTimer Timer8(8);

DueTimer::DueTimer(int _timer){
	/*
		The constructor of the class DueTimer 
	*/

	timer = _timer;
}

DueTimer DueTimer::getAvailable(){
	/*
		Return the first timer with no callback set
	*/

	for(int i = 0; i < 9; i++){
		if(!callbacks[i])
			return DueTimer(i);
	}
	// Default, return Timer0;
	return DueTimer(0);
}

DueTimer DueTimer::attachInterrupt(void (*isr)()){
	/*
		Links the function passed as argument to the timer of the object
	*/

	callbacks[timer] = isr;

	return *this;
}

DueTimer DueTimer::detachInterrupt(){
	/*
		Links the function passed as argument to the timer of the object
	*/

	stop(); // Stop the currently running timer

	callbacks[timer] = NULL;

	return *this;
}

DueTimer DueTimer::start(long microseconds){
	/*
		Start the timer
		If a period is set, then sets the period and start the timer
	*/

	if(microseconds > 0)
		setPeriod(microseconds);
	
	if(_frequency[timer] <= 0)
		setFrequency(1);

	NVIC_ClearPendingIRQ(Timers[timer].irq);
	NVIC_EnableIRQ(Timers[timer].irq);

	return *this;
}

DueTimer DueTimer::stop(){
	/*
		Stop the timer
	*/

	NVIC_DisableIRQ(Timers[timer].irq);

	return *this;
}

uint8_t DueTimer::bestClock(double frequency, uint32_t& retRC){
	/*
		Pick the best Clock, thanks to Ogle Basil Hall!

		Timer		Definition
		TIMER_CLOCK1	MCK /  2
		TIMER_CLOCK2	MCK /  8
		TIMER_CLOCK3	MCK / 32
		TIMER_CLOCK4	MCK /128
	*/
	struct {
		uint8_t flag;
		uint8_t divisor;
	} clockConfig[] = {
		{ TC_CMR_TCCLKS_TIMER_CLOCK1,   2 },
		{ TC_CMR_TCCLKS_TIMER_CLOCK2,   8 },
		{ TC_CMR_TCCLKS_TIMER_CLOCK3,  32 },
		{ TC_CMR_TCCLKS_TIMER_CLOCK4, 128 }
	};
	float ticks;
	float error;
	int clkId = 3;
	int bestClock = 3;
	float bestError = 1.0;
	do
	{
		ticks = (float) VARIANT_MCK / frequency / (float) clockConfig[clkId].divisor;
		error = abs(ticks - round(ticks));
		if (abs(error) < bestError)
		{
			bestClock = clkId;
			bestError = error;
		}
	} while (clkId-- > 0);
	ticks = (float) VARIANT_MCK / frequency / (float) clockConfig[bestClock].divisor;
	retRC = (uint32_t) round(ticks);
	return clockConfig[bestClock].flag;
}

bool DueTimer::setUpCounter() {
	/*
		Set up a TC channel as hardware counter
		driven by its external clock input.
	*/

	Timer t = Timers[timer];

	if (t.tclk_pin == NULL) return false;

	// Set up the external clock input 
	PIO_Configure(g_APinDescription[t.tclk_pin].pPort,
		t.tclk_periph,
		g_APinDescription[t.tclk_pin].ulPin,
		PIO_DEFAULT);

	// Tell the Power Management Controller to disable 
	// the write protection of the (Timer/Counter) registers:
	pmc_set_writeprotect(false);

	// Enable clock for the timer
	pmc_enable_periph_clk((uint32_t)t.irq);

	uint32_t xc_tclk, tcclk_xc;
	// Set up external clock input for the channel
	switch (t.channel) {
		case 0:
			xc_tclk = TC_BMR_TC0XC0S_TCLK0;
			tcclk_xc = TC_CMR_TCCLKS_XC0;
			break;
		case 1:
			xc_tclk = TC_BMR_TC1XC1S_TCLK1;
			tcclk_xc = TC_CMR_TCCLKS_XC1;
			break;
		case 2:
			xc_tclk = TC_BMR_TC2XC2S_TCLK2;
			tcclk_xc = TC_CMR_TCCLKS_XC2;
			break;
		default:
			return false;
	}

	t.tc->TC_BMR |= xc_tclk;
	TC_Configure(t.tc, t.channel, tcclk_xc | TC_CMR_BURST_NONE);

	return true;
}

DueTimer DueTimer::startCounter() {
	/*
		Start the counter (resets the counter value to zero)
	*/

	Timer t = Timers[timer];

	TC_Start(t.tc, t.channel);

	return *this;
}

DueTimer DueTimer::stopCounter() {
	/*
		Stop the counter
	*/

	Timer t = Timers[timer];

	TC_Stop(t.tc, t.channel);

	return *this;
}

uint32_t DueTimer::counterValue(){
	/*
		Get the current counter value
	*/

	Timer t = Timers[timer];

	return TC_ReadCV(t.tc, t.channel);
}

DueTimer DueTimer::setFrequency(double frequency){
	/*
		Set the timer frequency (in Hz)
	*/

	// Prevent negative frequencies
	if(frequency <= 0) { frequency = 1; }

	// Remember the frequency
	_frequency[timer] = frequency;

	// Get current timer configuration
	Timer t = Timers[timer];

	uint32_t rc = 0;
	uint8_t clock;

	// Tell the Power Management Controller to disable 
	// the write protection of the (Timer/Counter) registers:
	pmc_set_writeprotect(false);

	// Enable clock for the timer
	pmc_enable_periph_clk((uint32_t)t.irq);

	// Find the best clock for the wanted frequency
	clock = bestClock(frequency, rc);

	// Set up the Timer in waveform mode which creates a PWM
	// in UP mode with automatic trigger on RC Compare
	// and sets it up with the determined internal clock as clock input.
	TC_Configure(t.tc, t.channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | clock);
	// Reset counter and fire interrupt when RC value is matched:
	TC_SetRC(t.tc, t.channel, rc);
	// Start the Counter channel
	TC_Start(t.tc, t.channel);
	// Enable the RC Compare Interrupt...
	t.tc->TC_CHANNEL[t.channel].TC_IER=TC_IER_CPCS;
	// ... and disable all others.
	t.tc->TC_CHANNEL[t.channel].TC_IDR=~TC_IER_CPCS;

	return *this;
}

DueTimer DueTimer::setPeriod(long microseconds){
	/*
		Set the period of the timer (in microseconds)
	*/

	// Convert period in microseconds to frequency in Hz
	double frequency = 1000000.0 / microseconds;	
	setFrequency(frequency);
	return *this;
}

double DueTimer::getFrequency(){
	/*
		Get current time frequency
	*/

	return _frequency[timer];
}

long DueTimer::getPeriod(){
	/*
		Get current time period
	*/

	return 1.0/getFrequency()*1000000;
}


/*
	Implementation of the timer callbacks defined in 
	arduino-1.5.2/hardware/arduino/sam/system/CMSIS/Device/ATMEL/sam3xa/include/sam3x8e.h
*/
void TC0_Handler(){
	TC_GetStatus(TC0, 0);
	DueTimer::callbacks[0]();
}
void TC1_Handler(){
	TC_GetStatus(TC0, 1);
	DueTimer::callbacks[1]();
}
void TC2_Handler(){
	TC_GetStatus(TC0, 2);
	DueTimer::callbacks[2]();
}
void TC3_Handler(){
	TC_GetStatus(TC1, 0);
	DueTimer::callbacks[3]();
}
void TC4_Handler(){
	TC_GetStatus(TC1, 1);
	DueTimer::callbacks[4]();
}
void TC5_Handler(){
	TC_GetStatus(TC1, 2);
	DueTimer::callbacks[5]();
}
void TC6_Handler(){
	TC_GetStatus(TC2, 0);
	DueTimer::callbacks[6]();
}
void TC7_Handler(){
	TC_GetStatus(TC2, 1);
	DueTimer::callbacks[7]();
}
void TC8_Handler(){
	TC_GetStatus(TC2, 2);
	DueTimer::callbacks[8]();
}
