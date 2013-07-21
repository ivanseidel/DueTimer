# DueTimer

Timer Library to work with Arduino DUE

## Installation

1. [Download](https://github.com/ivanseidel/DueTimer/releases) the Latest release from gitHub.
2. Unzip and modify the Folder name to "DueTimer" (Remove the '-version')
3. Paste the modified folder on your Library folder (On your `Libraries` folder inside Sketchbooks or Arduino software).

## Getting Started

To call a function `handler` every `1000` microseconds:

```c++
Timer3.attachInterrupt(handler).start(1000);
// or:
Timer3.attachInterrupt(handler).setPeriod(1000).start();
// or, to select whichever available timer:
Timer.getAvailable().attachInterrupt(handler).start(1000);
```

To call a function `handler` `10` times a second:

```c++
Timer3.attachInterrupt(handler).setFrequency(10).start();
```

In case you need to stop a timer, just do like this:

```c++
Timer3.stop();
```

And to continue running:

```c++
Timer3.start();
```

There are `9` Timer objects already instantiated for you:
`Timer0`, `Timer1`, `Timer2`, `Timer3`, `Timer4`, `Timer5`, `Timer6`, `Timer7` and `Timer8`.

### TIPs and Warnings

```c++
Timer4.attatchInterrupt(handler).setFrequency(10).start();
// Is the same as:
Timer4.attatchInterrupt(handler);
Timer4.setFrequency(10);
Timer4.start();

// To create a custom timer, refer to:
DueTimer myTimer = DueTimer(0); // Creates a Timer 0 object.
DueTimer myTimer = DueTimer(3); // Creates a Timer 3 object.
DueTimer myTimer = DueTimer(t); // Creates a Timer t object.
// Note: Maximum t allowed is 8, as there is only 9 timers [0..8];

Timer1.setHandler(handler1).start(10);
Timer1.setHandler(handler2).start(10);
DueTimer myTimer = DueTimer(1);
myTimer.setHandler(handler3).start(20);
// Will run only handle3, on Timer 1 (You are just overriding the callback)

Timer.getAvailable().attachInterrupt(callback1).start(10);
// Start timer on first available timer
DueTimer::getAvailable().attachInterrupt(callback2).start(10);
// Start timer on second available timer
// And so on...
```

## Library Reference

### You should know:

- `getAvailable()` - Get the first available Timer.

- `attachInterrupt(void (*isr)())` - Attach a interrupt (callback function) for the timer of the object.

- `detachInterrupt()` - Detach current callback of timer.

- `start(long microseconds = -1)` - Start the timer with an optional period parameter.

- `stop()` - Stop the timer

- `setFrequency(long frequency)` - Set the timer frequency

- `long getFrequency()` - Get the timer frequency

- `setPeriod(long microseconds)` - Set the timer period (in microseconds)

- `long getPeriod()` - Get the timer period (in microseconds)

### You don't need to know:

- `int timer` - Stores the object timer id (to acces Timers struct array).

- `DueTimer(int _timer)` - Instantiate a new DueTimer object for Timer _timer (NOTE: All objects are already instantiated!).

- `static const Timer Timers[]` - Stores all timers information

- `static void (*callbacks[])()` - Stores all callbacks for all timers


### Hardware Information

More information on the Timer Counter module of the µC on the Arduino Due
can be found in the documentation file [TimerCounter](TimerCounter.md).

### Version History

* `1.2 (2013-30-03)`: Clock selection. Getters. `getAvailable()`. "AvailableTimer" Example.
* `1.1 (2013-30-03)`: Added Timer6, Timer7, Timer8 (TC2).
* `1.0 (2013-30-03)`: Original release.

![DueTimer](https://d2weczhvl823v0.cloudfront.net/ivanseidel/DueTimer/trend.png)
