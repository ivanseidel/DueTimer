#include <DueTimer.h>

void playVideogame(){
	Serial.println("[-        ] I'm playing Videogame!");
}

void drinkWater(){
	Serial.println("[ -       ] I'm driking water!");
}

void makeSushi(){
	Serial.println("[  -      ] Slicing Salmon...");
}

void singOnShower(){
	Serial.println("[   -     ] Hello World! Hello world!");
}

void studyMath(){
	int x = random(1, 40), y = random(1,40);
	Serial.print("[    -    ] ");
	Serial.print(x); Serial.print(" x "); Serial.print(y); Serial.print(" = ");
	Serial.println(x*y);
}

void watchStarTrek(){
	Serial.println("[     -   ] Long live and prosper \\\\//_");
}

void eatSushi(){
	Serial.println("[      -  ] ...");
}

void readTextMessage(){
	Serial.println("[       - ] [Unlock ---->>]");
}

void goToSleep(){
	Serial.println("[        -] zzzzzz");
}

void setup(){
	Serial.begin(9600);

	Timer.getAvailable().attachInterrupt(playVideogame).start(); delay(50);
	Timer.getAvailable().attachInterrupt(drinkWater).start(); delay(50);
	Timer.getAvailable().attachInterrupt(makeSushi).start(); delay(50);
	Timer.getAvailable().attachInterrupt(singOnShower).start(); delay(50);
	Timer.getAvailable().attachInterrupt(studyMath).start(); delay(50);
	Timer.getAvailable().attachInterrupt(watchStarTrek).start(); delay(50);
	Timer.getAvailable().attachInterrupt(eatSushi).start(); delay(50);
	Timer.getAvailable().attachInterrupt(readTextMessage).start(); delay(50);
	Timer.getAvailable().attachInterrupt(goToSleep).start(); delay(50);
}

void loop(){
	while(1){
		// ...
	}
}