/*
  DueTimer.cpp - Implementation of Timers defined on DueTimer.h
  For instructions, go to https://github.com/ivanseidel/DueTimer

  Created by Ivan Seidel Gomes, March, 2013.
  Released into the public domain.
*/

#include "DueTimer.h"

const DueTimer::Timer DueTimer::Timers[6] = {
	{TC0,0,TC0_IRQn},
	{TC0,1,TC1_IRQn},
	{TC0,2,TC2_IRQn},
	{TC1,0,TC3_IRQn},
	{TC1,1,TC4_IRQn},
	{TC1,2,TC5_IRQn}
};

void (*DueTimer::callbacks[6])() = {
	NULL, NULL, NULL, NULL, NULL, NULL
};

/*
	Initialize all timers, so you can use it like: Timer0.start();
*/
DueTimer Timer0(0);
DueTimer Timer1(1);
DueTimer Timer2(2);
DueTimer Timer3(3);
DueTimer Timer4(4);
DueTimer Timer5(5);

// Constructor
DueTimer::DueTimer(int _timer){
	timer = _timer;

	// Initialize timer
	Timer t = Timers[timer];
	pmc_set_writeprotect(false);
	pmc_enable_periph_clk((uint32_t)Timers[timer].irq);
    TC_Configure(t.tc, t.channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK4);
}


// Links the function passed as argument to the timer of the object
DueTimer DueTimer::attachInterrupt(void (*isr)()){
	callbacks[timer] = isr;
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

// Set the frequency (in Hz)
DueTimer DueTimer::setFrequency(long frequency){
	Timer t = Timers[timer];
    uint32_t rc = VARIANT_MCK/128/frequency; //128 because we selected TIMER_CLOCK4 above
    TC_SetRA(t.tc, t.channel, rc/2); //50% high, 50% low
    TC_SetRC(t.tc, t.channel, rc);
    TC_Start(t.tc, t.channel);
    t.tc->TC_CHANNEL[t.channel].TC_IER=TC_IER_CPCS;
    t.tc->TC_CHANNEL[t.channel].TC_IDR=~TC_IER_CPCS;
	return *this;
}

// Set the period of the timer (in microseconds)
DueTimer DueTimer::setPeriod(long microseconds){
	setFrequency(1000000/microseconds); // Convert from period in microseconds to frequency
	return *this;
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