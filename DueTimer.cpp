/*
  DueTimer.cpp - Implementation of Timers defined on DueTimer.h
  For instructions, go to https://github.com/ivanseidel/DueTimer

  Created by Ivan Seidel Gomes, March, 2013.
  Thanks to stimmer (from Arduino forum), for coding the "timer soul" (Register stuff)
  Released into the public domain.
*/

#include "DueTimer.h"

const DueTimer::Timer DueTimer::Timers[9] = {
	{TC0,0,TC0_IRQn},
	{TC0,1,TC1_IRQn},
	{TC0,2,TC2_IRQn},
	{TC1,0,TC3_IRQn},
	{TC1,1,TC4_IRQn},
	{TC1,2,TC5_IRQn},
	{TC2,0,TC6_IRQn},
	{TC2,1,TC7_IRQn},
	{TC2,2,TC8_IRQn},
};

void (*DueTimer::callbacks[9])() = {};
double DueTimer::_frequency[9] = {1,1,1,1,1,1,1,1,1};

/*
	Initialize all timers, so you can use it like: Timer0.start();
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

// Constructor
DueTimer::DueTimer(int _timer){
	timer = _timer;

	// Initialize Default frequency
	setFrequency(_frequency[_timer]);
}

// Get the first timer without any callbacks
DueTimer DueTimer::getAvailable(){
	for(int i = 0; i < 9; i++){
		if(!callbacks[i])
			return DueTimer(i);
	}
	// Default, return Timer0;
	return DueTimer(0);
}

// Links the function passed as argument to the timer of the object
DueTimer DueTimer::attachInterrupt(void (*isr)()){
	callbacks[timer] = isr;

	return *this;
}

// Links the function passed as argument to the timer of the object
DueTimer DueTimer::detachInterrupt(){
	// Stop running timer
	stop();

	callbacks[timer] = NULL;

	return *this;
}

// Start the timer
// If a period is set, then sets the period and start the timer
DueTimer DueTimer::start(long microseconds){
	if(microseconds > 0)
		setPeriod(microseconds);

    NVIC_EnableIRQ(Timers[timer].irq);

	return *this;
}

// Stop the timer
DueTimer DueTimer::stop(){
	NVIC_DisableIRQ(Timers[timer].irq);

	return *this;
}

// Pick the best Clock
// It uses Black magic to do this, thanks to Ogle Basil Hall!
uint8_t DueTimer::bestClock(double frequency, uint32_t& retRC){
	/*
	    Timer		Definition
	    TIMER_CLOCK1	MCK/2
	    TIMER_CLOCK2	MCK/8
	    TIMER_CLOCK3	MCK/32
	    TIMER_CLOCK4	MCK/128
	*/
	struct {
		uint8_t flag;
		uint8_t divisor;
	} clockConfig[] = {
		{ TC_CMR_TCCLKS_TIMER_CLOCK1, 2 },
		{ TC_CMR_TCCLKS_TIMER_CLOCK2, 8 },
		{ TC_CMR_TCCLKS_TIMER_CLOCK3, 32 },
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


// Set the frequency (in Hz)
DueTimer DueTimer::setFrequency(double frequency){
	//prevent negative frequencies
	if(frequency <= 0) { frequency = 1; }

	// Saves current frequency
	_frequency[timer] = frequency;

	// Get current timer configurations
	Timer t = Timers[timer];

	uint32_t rc = 0;
	uint8_t clock;

	// Yes, we don't want pmc protected!
	pmc_set_writeprotect(false);

	// Enable clock for the timer
	pmc_enable_periph_clk((uint32_t)Timers[timer].irq);

	// Do magic, and find's best clock
	clock = bestClock(frequency, rc);
    TC_Configure(t.tc, t.channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | clock);

    // Pwm stuff
    TC_SetRA(t.tc, t.channel, rc/2); //50% high, 50% low
    TC_SetRC(t.tc, t.channel, rc);
    TC_Start(t.tc, t.channel);
    t.tc->TC_CHANNEL[t.channel].TC_IER=TC_IER_CPCS;
    t.tc->TC_CHANNEL[t.channel].TC_IDR=~TC_IER_CPCS;

	return *this;
}

// Set the period of the timer (in microseconds)
DueTimer DueTimer::setPeriod(long microseconds){
	double frequency = 1000000.0 / microseconds;	
	setFrequency(frequency); // Convert from period in microseconds to frequency in Hz
	return *this;
}

// Get current time frequency
double DueTimer::getFrequency(){
	return _frequency[timer];
}

// Get current time period
long DueTimer::getPeriod(){
	return 1.0/getFrequency()*1000000;
}


/*
	Default timers callbacks
	DO NOT CHANGE!
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
