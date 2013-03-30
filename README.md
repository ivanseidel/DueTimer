h1. DueTimer

Timer Library to work with Arduino DUE

h2. Getting Started

To call a function @handler@ every @1000@ microseconds:
pre.
	Timer3.attachInterrupt(handler).setPeriod(1000).start();

To call a function @handler@ 10 times a second:
pre.
	Timer3.attachInterrupt(handler).start(10);
	or:
	Timer3.attachInterrupt(handler).setFrequency(10).start();

In case you need to stop a timer, just do like this:
pre. 
	Timer3.stop();

... and to continue:
pre.
	Timer3.start();

There are 6 Timer objects already instantied for you:
@Timer0@, @Timer1@, @Timer2@, @Timer3@, @Timer4@ and @Timer5@.

	