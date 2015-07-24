# The Timer Counter blocks of Arduino Due's AT91SAM3X8E

The AT91SAM3X8E embedds nine general-purpose 32-bit timers/counters. They are organized in three blocks (`TC0`, `TC1`, `TC2`) each containing three channels (`0`, `1`, `2`).
Each block and channel has input lines for their clock and input/output lines which can be used in different ways, such as PWM pins etc.


### Resources

Information on the Timer Counter and its I/O signals pins can be found here:

* Section **37 Timer Counter (TC)** in the [Atmel SAM3X Datasheet][]  
  You find this chapter on the pages 869 through 920. It explains in detail how to operate the counter in different modes and has nice diagrams that show you how the clock input works and how to set up interrupts based on counter values etc.
* [Arduino Due's SAM3X Pin Mapping][] and the 
* [Arduino Due pinout diagram][].

### The Timer Counter Signals / Pins

Many of the I/O lines of the Timer Counter are directly accessible on pin headers of the Arduino Due board. This section helps you to find out which of them are and which are not.

In order to actually connect those signals (`TCLKx`, `TIOAx` and `TIOBx`) to the pins on the Arduino Due, you must tell the Parallel I/O Controllers (`PIOA`, `PIOB`, `PIOC` or `PIOD`) to set up their mux (multiplexer) accordingly.<br />
A useful resource to find out how to set the PIOs is
[Arduino's const array of PinDescriptions](https://github.com/arduino/Arduino/blob/ide-1.5.x/hardware/arduino/sam/variants/arduino_due_x/variant.cpp#L117) and the 
[definition of PinDescription](https://github.com/arduino/Arduino/blob/ide-1.5.x/hardware/arduino/sam/cores/arduino/Arduino.h#L166), the data type of its entries.

#### Overview of the I/O lines of the Timer Counter

Here is a table of the Timer Counter channels and their `TCLKx` (external clock input), `TIOAx` (I/O Line _A_) and `TIOBx` (I/O Line _B_) signals:

Instance | TC  | Channel| External Clock Input | I/O Line A | I/O Line B
---------|-----|--------|----------------------|------------|-----------
 T0      | TC0 |   0    |         TCLK0        |   TIOA0    |    TIOB0
 T1      | TC0 |   1    |         TCLK1        |   TIOA1    |    TIOB1
 T2      | TC0 |   2    |         TCLK2        |   TIOA2    |    TIOB2
 T3      | TC1 |   0    |         TCLK3        |   TIOA3    |    TIOB3
 T4      | TC1 |   1    |         TCLK4        |   TIOA4    |    TIOB4
 T5      | TC1 |   2    |         TCLK5        |   TIOA5    |    TIOB5
 T6      | TC2 |   0    |         TCLK6        |   TIOA6    |    TIOB6
 T7      | TC2 |   1    |         TCLK7        |   TIOA7    |    TIOB7
 T8      | TC2 |   2    |         TCLK8        |   TIOA8    |    TIOB8

##### TCLKx

Here is how the **external clock inputs** are routed to pins on the Arduino Due board:

Clock Input | Port Pin of µC | Pin on Arduino Due Board
------------|------------|-----------------------------
TCLK0       |    PB 26   |   Digital Pin 22
TCLK1       |    PA  4   |      Analog In 5
TCLK2       |    PA  7   |   Digital Pin 31
TCLK3       |    PA 22   |      Analog In 3
TCLK4       |    PA 23   |      Analog In 2
TCLK5       |    PB 16   |             DAC1
TCLK6       |    PC 27   |        /
TCLK7       |    PC 30   |         LED "RX"
TCLK8       |    PD  9   |   Digital Pin 30

##### TIOAx

Here is how the **I/O Lines _A_** are routed to pins on the Arduino Due board:

I/O Line A | Port Pin of µC | Pin on Arduino Due Board
-----------|----------------|-----------------------------
TIOA0      |    PB 25       |   Digital Pin 2
TIOA1      |    PA  2       |     Analog In 7
TIOA2      |    PA  5       |   /
TIOA3      |    PE  9       |   /
TIOA4      |    PE 11       |   /
TIOA5      |    PE 13       |   /
TIOA6      |    PC 25       |   Digital Pin  5
TIOA7      |    PC 28       |   Digital Pin  3
TIOA8      |    PD  7       |   Digital Pin 11

##### TIOBx

Here is how the **I/O Lines _B_** are routed to pins on the Arduino Due board:

I/O Line B|Port Pin of µC| Pin on Arduino Due Board
----------|--------------|------------------------------------------
TIOB0     |     PB 27    |   Digital Pin 13 / Amber LED "L"
TIOB1     |     PA  3    |   Analog In 6
TIOB2     |     PA  6    |   Analog In 4
TIOB3     |     PE 10    |   /
TIOB4     |     PE 12    |   /
TIOB5     |     PE 14    |   /
TIOB6     |     PC 26    |   Digital Pin  4 (also connected to PA29)
TIOB7     |     PC 29    |   Digital Pin 10 (also connected to PA28)
TIOB8     |     PD  8    |   Digital Pin 12

#### TC Clock Source Selection

The *clock of the counters* can be set very flexibly to internal or external signals. To *use the channel as a timer*, it makes sense to connect its clock input to an internal timer clock. Here are the options:

Name         | Definition
-------------|-----------
TIMER_CLOCK1 | `MCK /   2`
TIMER_CLOCK2 | `MCK /   8`
TIMER_CLOCK3 | `MCK /  32`
TIMER_CLOCK4 | `MCK / 128`
TIMER_CLOCK5 | `SLCK`


Where `MCK` is the master clock (84 MHz for the Arduino Due) and SLCK is the slow clock (which can be clocked at 32.768 kHz by Y2 on the Arduino Due).

[Atmel SAM3X Datasheet]: http://www.atmel.com/Images/Atmel-11057-32-bit-Cortex-M3-Microcontroller-SAM3X-SAM3A_Datasheet.pdf
[Arduino Due's SAM3X Pin Mapping]: http://arduino.cc/en/Hacking/PinMappingSAM3X
[Arduino Due pinout diagram]: http://www.robgray.com/temp/Due-pinout.pdf
