/*
  DueTimer.cpp - Implementation of Timers defined on DueTimer.h
  For instructions, go to https://github.com/ivanseidel/DueTimer

  Created by Ivan Seidel Gomes, March, 2013.
  Modified by Philipp Klaus, June 2013.
  Thanks to stimmer (from Arduino forum), for coding the "timer soul" (Register stuff)
  Released into the public domain.
*/

#include <Arduino.h>
#if defined(_SAM3XA_)
#include "DueTimer.h"

const DueTimer::Timer DueTimer::Timers[NUM_TIMERS] = {
	{TC0,0,TC0_IRQn, PIOB, PIO_PER_P25, PIO_PERIPH_B, PIOB, PIO_PER_P27, PIO_PERIPH_B, PIOB, PIO_PER_P26, PIO_PERIPH_B},
	{TC0,1,TC1_IRQn, PIOA, PIO_PER_P2 , PIO_PERIPH_A, PIOA, PIO_PER_P3 , PIO_PERIPH_A, PIOA, PIO_PER_P4, PIO_PERIPH_A},
	{TC0,2,TC2_IRQn, PIOA, PIO_PER_P5 , PIO_PERIPH_A, PIOA, PIO_PER_P6 , PIO_PERIPH_A, PIOA, PIO_PER_P7, PIO_PERIPH_A},
	
	{TC1,0,TC3_IRQn, PIOB, PIO_PER_P0 , PIO_PERIPH_B, PIOB, PIO_PER_P1 , PIO_PERIPH_B, PIOA, PIO_PER_P22, PIO_PERIPH_B},
	{TC1,1,TC4_IRQn, PIOB, PIO_PER_P2 , PIO_PERIPH_B, PIOB, PIO_PER_P3 , PIO_PERIPH_B, PIOA, PIO_PER_P23, PIO_PERIPH_B},
	{TC1,2,TC5_IRQn, PIOB, PIO_PER_P4 , PIO_PERIPH_B, PIOB, PIO_PER_P5 , PIO_PERIPH_B, PIOB, PIO_PER_P16, PIO_PERIPH_A},
	
	{TC2,0,TC6_IRQn, PIOC, PIO_PER_P25, PIO_PERIPH_B, PIOC, PIO_PER_P26 , PIO_PERIPH_B, PIOC, PIO_PER_P27, PIO_PERIPH_B},
	{TC2,1,TC7_IRQn, PIOC, PIO_PER_P28, PIO_PERIPH_B, PIOC, PIO_PER_P29 , PIO_PERIPH_B, PIOC, PIO_PER_P30, PIO_PERIPH_B},
	{TC2,2,TC8_IRQn, PIOD, PIO_PER_P7 , PIO_PERIPH_B, PIOD, PIO_PER_P8 , PIO_PERIPH_B, PIOD, PIO_PER_P9, PIO_PERIPH_B},
};

// Fix for compatibility with Servo library
#ifdef USING_SERVO_LIB
	// Set callbacks as used, allowing DueTimer::getAvailable() to work
	void (*DueTimer::callbacks[NUM_TIMERS])() = {
		(void (*)()) 1, // Timer 0 - Occupied
		(void (*)()) 0, // Timer 1
		(void (*)()) 1, // Timer 2 - Occupied
		(void (*)()) 1, // Timer 3 - Occupied
		(void (*)()) 1, // Timer 4 - Occupied
		(void (*)()) 1, // Timer 5 - Occupied
		(void (*)()) 0, // Timer 6
		(void (*)()) 0, // Timer 7
		(void (*)()) 0  // Timer 8
	};
#else
	void (*DueTimer::callbacks[NUM_TIMERS])() = {};
#endif
double DueTimer::_frequency[NUM_TIMERS] = {-1,-1,-1,-1,-1,-1,-1,-1,-1};
uint32_t DueTimer::_statusRegister[NUM_TIMERS] = {0,0,0,0,0,0,0,0};

/*
	Initializing all timers, so you can use them like this: Timer0.start();
*/
DueTimer Timer(0);

DueTimer Timer1(1);
// Fix for compatibility with Servo library
#ifndef USING_SERVO_LIB
	DueTimer Timer0(0);
	DueTimer Timer2(2);
	DueTimer Timer3(3);
	DueTimer Timer4(4);
	DueTimer Timer5(5);
#endif
DueTimer Timer6(6);
DueTimer Timer7(7);
DueTimer Timer8(8);

DueTimer::DueTimer(unsigned short _timer) : timer(_timer){
	/*
		The constructor of the class DueTimer 
	*/
}

DueTimer DueTimer::getAvailable(void){
	/*
		Return the first timer with no callback set
	*/

	for(int i = 0; i < NUM_TIMERS; i++){
		if(!callbacks[i])
			return DueTimer(i);
	}
	// Default, return Timer0;
	return DueTimer(0);
}

DueTimer& DueTimer::attachInterrupt(void (*isr)()){
	/*
		Links the function passed as argument to the timer of the object
	*/

	callbacks[timer] = isr;

	return *this;
}

DueTimer& DueTimer::detachInterrupt(void){
	/*
		Links the function passed as argument to the timer of the object
	*/

	stop(); // Stop the currently running timer

	callbacks[timer] = NULL;

	return *this;
}

DueTimer& DueTimer::start(void){
	/*
		Start the timer
	*/
	if (callbacks[timer]) {
		NVIC_ClearPendingIRQ(Timers[timer].irq);
		NVIC_EnableIRQ(Timers[timer].irq);
	}
	TC_Start(Timers[timer].tc, Timers[timer].channel);

	return *this;
}

DueTimer& DueTimer::start(double microseconds){
	/*
		Start the timer
		If a period is set, then sets the period and start the timer
	*/
	if(microseconds > 0)
		setPeriod(microseconds);
	if(_frequency[timer] <= 0)
		setFrequency(1);
	start();
	return *this;
}

DueTimer& DueTimer::stop(void){
	/*
		Stop the timer
	*/

	if (callbacks[timer]) {
		NVIC_DisableIRQ(Timers[timer].irq);
	}
	TC_Stop(Timers[timer].tc, Timers[timer].channel);

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
	const struct {
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
	float bestError = 9.999e99;
	do
	{
		ticks = (float) VARIANT_MCK / frequency / (float) clockConfig[clkId].divisor;
		// error = abs(ticks - round(ticks));
		error = clockConfig[clkId].divisor * abs(ticks - round(ticks));	// Error comparison needs scaling
		if (error < bestError)
		{
			bestClock = clkId;
			bestError = error;
		}
	} while (clkId-- > 0);
	ticks = (float) VARIANT_MCK / frequency / (float) clockConfig[bestClock].divisor;
	retRC = (uint32_t) round(ticks);
	return clockConfig[bestClock].flag;
}


DueTimer& DueTimer::setFrequency(double frequency){
	/*
		Set the timer frequency (in Hz)
	*/

	// Prevent negative frequencies
	if(frequency <= 0) { frequency = 1; }

	// Remember the frequency — see below how the exact frequency is reported instead
	//_frequency[timer] = frequency;

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

	switch (clock) {
	  case TC_CMR_TCCLKS_TIMER_CLOCK1:
	    _frequency[timer] = (double)VARIANT_MCK / 2.0 / (double)rc;
	    break;
	  case TC_CMR_TCCLKS_TIMER_CLOCK2:
	    _frequency[timer] = (double)VARIANT_MCK / 8.0 / (double)rc;
	    break;
	  case TC_CMR_TCCLKS_TIMER_CLOCK3:
	    _frequency[timer] = (double)VARIANT_MCK / 32.0 / (double)rc;
	    break;
	  default: // TC_CMR_TCCLKS_TIMER_CLOCK4
	    _frequency[timer] = (double)VARIANT_MCK / 128.0 / (double)rc;
	    break;
	}

	// Set up the Timer in waveform mode which creates a PWM
	// in UP mode with automatic trigger on RC Compare
	// and sets it up with the determined internal clock as clock input.
	TC_Configure(t.tc, t.channel, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_ACPA_SET | TC_CMR_ACPC_CLEAR | TC_CMR_BCPB_SET | TC_CMR_BCPC_CLEAR | TC_CMR_EEVT_XC0| clock);
	// Reset counter and fire interrupt when RC value is matched:
	TC_SetRC(t.tc, t.channel, rc);
	// Configure RA and RB for a 50% duty cycle
	TC_SetRA(t.tc, t.channel, rc >> 1);
	TC_SetRB(t.tc, t.channel, rc >> 1);
	// Enable the RC Compare Interrupt...
	t.tc->TC_CHANNEL[t.channel].TC_IER=TC_IER_CPCS;
	// ... and disable all others.
	t.tc->TC_CHANNEL[t.channel].TC_IDR=~TC_IER_CPCS;

	return *this;
}

DueTimer& DueTimer::enablePinA() {
	// Configure TIOA 

	// disable pioa protection (useless but...)
	Timers[timer].pioa->PIO_WPMR = 0x50494F00;

	PIO_Configure(
		Timers[timer].pioa,
		Timers[timer].tioaPeriph,
		Timers[timer].tioaBits,
		PIO_DEFAULT
	);
	return *this;
}
DueTimer& DueTimer::enablePinB() {
	// Configure TIOB

	// disable piob protection
	Timers[timer].piob->PIO_WPMR = 0x50494F00;

	PIO_Configure(
		Timers[timer].piob,
		Timers[timer].tiobPeriph,
		Timers[timer].tiobBits,
		PIO_DEFAULT
	);
	return *this;
}
DueTimer& DueTimer::enablePinClock() {
	// Configure CLK pin

	// disable pio protection
	Timers[timer].piob->PIO_WPMR = 0x50494F00;

	PIO_Configure(
		Timers[timer].pioclk,
		Timers[timer].tioClkPeriph,
		Timers[timer].tioClkBits,
		PIO_DEFAULT
	);
	return *this;
}

DueTimer& DueTimer::setPeriod(double microseconds){
	/*
		Set the period of the timer (in microseconds)
	*/
	// Convert period in microseconds to frequency in Hz
	double frequency = 1000000.0 / microseconds;	
	setFrequency(frequency);
	return *this;
}

uint32_t DueTimer::getDutyCycleRX(double percent){
	/*
		get the corresponding R of a given duty cycle.
	*/
	Timer t = Timers[timer];
	uint32_t rc;
	// Actual RC
	rc = t.tc->TC_CHANNEL[t.channel].TC_RC;
	
	if (percent>=100.0) rc = 0; else rc = (uint32_t) ((1-percent) * rc);
	return rc;
}

DueTimer& DueTimer::setDutyCycleA(double percent){
	/*
		Set the duty cycle of the TIOA 
	*/
	Timer t = Timers[timer];
	TC_SetRA(t.tc, t.channel, getDutyCycleRX(percent));
	return *this;
}

DueTimer& DueTimer::setDutyCycleB(double percent){
	/*
		Set the duty cycle of the TIOB
	*/
	Timer t = Timers[timer];
	TC_SetRB(t.tc, t.channel, getDutyCycleRX(percent));
	return *this;
}

double DueTimer::getFrequency(void) const {
	/*
		Get current time frequency
	*/
	return _frequency[timer];
}

double DueTimer::getPeriod(void) const {
	/*
		Get current time period (microsecond)
	*/
	return 1.0e6 / getFrequency();
}

uint8_t DueTimer::bestCaptureClock(double maxPeriodMicroSeconds, double& resolution, uint32_t& maxPeriodRC){
	/*
		Pick the best capture Clock
		Timer		Definition
		TIMER_CLOCK1	MCK /  2
		TIMER_CLOCK2	MCK /  8
		TIMER_CLOCK3	MCK / 32
		TIMER_CLOCK4	MCK /128
		TIMER_CLOCK5	SCLK		 32.768Khz
	*/
	const struct {
		uint8_t flag;
		uint8_t divisor;
	} clockConfig[] = {
		{ TC_CMR_TCCLKS_TIMER_CLOCK1,   2 },
		{ TC_CMR_TCCLKS_TIMER_CLOCK2,   8 },
		{ TC_CMR_TCCLKS_TIMER_CLOCK3,  32 },
		{ TC_CMR_TCCLKS_TIMER_CLOCK4, 128 }
	};
	double tick; // µsecond
	double range = 4294967295; // 32 bits

	for (int idx=0; idx<4; idx++) {
		tick = 1e6 * (float) clockConfig[idx].divisor / (float) VARIANT_MCK;
		if (range * tick > maxPeriodMicroSeconds ) {
			resolution = tick;
			maxPeriodRC = 1 + (uint32_t) (maxPeriodMicroSeconds / tick);
			return clockConfig[idx].flag;
		}
	}
	resolution = tick;
	maxPeriodRC = 1 + (uint32_t) (maxPeriodMicroSeconds / tick);
	return TC_CMR_TCCLKS_TIMER_CLOCK4;
}

DueTimer& DueTimer::setCapture(double maxPeriodMicroSeconds){

	Timer t = Timers[timer];
	double resolution = 0;
	uint32_t periodRC = 0xFFFFFFFF;
	uint8_t clock;

	// Tell the Power Management Controller to disable 
	// the write protection of the (Timer/Counter) registers:
	pmc_set_writeprotect(false);
	// Enable clock for the timer
	pmc_enable_periph_clk((uint32_t)t.irq);

	// Find the best clock for the wanted frequency
	clock = bestCaptureClock(maxPeriodMicroSeconds, resolution, periodRC);
	// store the counter resolution
    _frequency[timer] = resolution;
	
	// Set up the Timer in capture mode 
	// TIOA : falling edge 	TC_CMR_LDRA_FALLING
	// TIOB : rising edge	TC_CMR_LDRB_RISING
	// No trigger condition (free run)
	
	// RA on falling, RB on rising (RB must be loaded after RA)
	TC_Configure(t.tc, t.channel,  TC_CMR_LDRA_FALLING | TC_CMR_LDRB_RISING | TC_CMR_CPCTRG |clock);
	// Enable the LOAD RA,RB,RC and OverFlow Interrupt...
	t.tc->TC_CHANNEL[t.channel].TC_IER = (TC_IER_LDRAS | TC_IER_LDRBS | TC_IER_COVFS | TC_IER_CPCS);
	// ... and disable all others.
	t.tc->TC_CHANNEL[t.channel].TC_IDR = ~(TC_IER_LDRAS | TC_IER_LDRBS | TC_IER_COVFS | TC_IER_CPCS);

	// Max period reset trigger
	TC_SetRC(t.tc, t.channel, periodRC);

	// Configure TIOA as input
	enablePinA();

	return *this;
}

DueTimer& DueTimer::setCounter(void){

	Timer t = Timers[timer];
	uint32_t mask,reg,clock;

	// Tell the Power Management Controller to disable 
	// the write protection of the (Timer/Counter) registers:
	pmc_set_writeprotect(false);
	// Enable clock for the timer
	pmc_enable_periph_clk((uint32_t)t.irq);

	// store the counter resolution
    _frequency[timer] = 0;
	
	// configure the block clock selction
	switch (t.channel) {
	 case 0 : mask = TC_BMR_TC0XC0S_Msk; reg = TC_BMR_TC0XC0S_TCLK0; clock = TC_CMR_TCCLKS_XC0; break; // XC0:TCLK0
	 case 1 : mask = TC_BMR_TC1XC1S_Msk; reg = TC_BMR_TC1XC1S_TCLK1; clock = TC_CMR_TCCLKS_XC1; break; // XC1:TCLK1
	 case 2 : mask = TC_BMR_TC2XC2S_Msk; reg = TC_BMR_TC2XC2S_TCLK2; clock = TC_CMR_TCCLKS_XC2; break; // XC2:TCLK2
	}
	t.tc->TC_BMR &= ~mask;
	t.tc->TC_BMR |= reg; 
	
	// Set up the Timer in WAVE mode, external clock
	TC_Configure(t.tc, t.channel,  TC_CMR_WAVE | clock);
	// Enable the LOAD RA,RB,RC and OverFlow Interrupt...
	t.tc->TC_CHANNEL[t.channel].TC_IER = (TC_IER_COVFS);
	// ... and disable all others.
	t.tc->TC_CHANNEL[t.channel].TC_IDR = ~(TC_IER_COVFS);

	// Configure Clk Input
	enablePinClock();
	
	return *this;
}

double DueTimer::getResolutionMicroSeconds(void) const {
	/*
		Get current capture resolution
	*/
	return _frequency[timer];
}

uint32_t DueTimer::statusRegister(void) {
	// Get the status register of the timer
	//Timer t = Timers[timer];
	//return t.tc->TC_CHANNEL[t.channel].TC_SR;
	return _statusRegister[timer];
}
uint32_t DueTimer::counterValue(void) {
	// Get the value of the counter
	Timer t = Timers[timer];
	return t.tc->TC_CHANNEL[t.channel].TC_CV;
}
uint32_t DueTimer::captureValueA(void) {
	// Get the value of the RegA register of the timer
	Timer t = Timers[timer];
	return t.tc->TC_CHANNEL[t.channel].TC_RA;
}
uint32_t DueTimer::captureValueB(void) {
	// Get the value of the RegB register of the timer
	Timer t = Timers[timer];
	return t.tc->TC_CHANNEL[t.channel].TC_RB;
}
uint32_t DueTimer::counterValueAndReset(void) {
	// Get the value of the counter and reset
	uint32_t val;
	Timer t = Timers[timer];
	val = t.tc->TC_CHANNEL[t.channel].TC_CV;
	t.tc->TC_CHANNEL[t.channel].TC_CCR = TC_CCR_SWTRG;
	return val;
}
DueTimer& DueTimer::resetCounterValue(void) {
	// Reset the counter (software trigger)
	Timer t = Timers[timer];
	t.tc->TC_CHANNEL[t.channel].TC_CCR = TC_CCR_SWTRG;
	return *this;
}
double DueTimer::valueToMicroSeconds(uint32_t value) {
	return value * getResolutionMicroSeconds();
}
double DueTimer::valueToMilliSeconds(uint32_t value) {
	return valueToMicroSeconds(value) / 1000.0;
}
double DueTimer::valueToSeconds(uint32_t value) {
	return valueToMicroSeconds(value) / 1000000.0;
}

/*
	Implementation of the timer callbacks defined in 
	arduino-1.5.2/hardware/arduino/sam/system/CMSIS/Device/ATMEL/sam3xa/include/sam3x8e.h
*/
// Fix for compatibility with Servo library
#ifndef USING_SERVO_LIB
void TC0_Handler(void){
	DueTimer::_statusRegister[0] = TC_GetStatus(TC0, 0);
	if (DueTimer::callbacks[0]) DueTimer::callbacks[0]();
}
#endif
void TC1_Handler(void){
	DueTimer::_statusRegister[1] = TC_GetStatus(TC0, 1);
	if (DueTimer::callbacks[1]) DueTimer::callbacks[1]();
}
// Fix for compatibility with Servo library
#ifndef USING_SERVO_LIB
void TC2_Handler(void){
	DueTimer::_statusRegister[2] = TC_GetStatus(TC0, 2);
	if (DueTimer::callbacks[2]) DueTimer::callbacks[2]();
}
void TC3_Handler(void){
	DueTimer::_statusRegister[3] = TC_GetStatus(TC1, 0);
	if (DueTimer::callbacks[3]) DueTimer::callbacks[3]();
}
void TC4_Handler(void){
	DueTimer::_statusRegister[4] = TC_GetStatus(TC1, 1);
	if (DueTimer::callbacks[4]) DueTimer::callbacks[4]();
}
void TC5_Handler(void){
	DueTimer::_statusRegister[5] = TC_GetStatus(TC1, 2);
	if (DueTimer::callbacks[5]) DueTimer::callbacks[5]();
}
#endif
void TC6_Handler(void){
	DueTimer::_statusRegister[6] = TC_GetStatus(TC2, 0);
	if (DueTimer::callbacks[6]) DueTimer::callbacks[6]();
}
void TC7_Handler(void){
	DueTimer::_statusRegister[7] = TC_GetStatus(TC2, 1);
	if (DueTimer::callbacks[7]) DueTimer::callbacks[7]();
}
void TC8_Handler(void){
	DueTimer::_statusRegister[8] = TC_GetStatus(TC2, 2);
	if (DueTimer::callbacks[8]) DueTimer::callbacks[8]();
}
#endif
