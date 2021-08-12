#include <Arduino.h>
#include "src/Spencer.h"
Spencer spencer;

void setup(){
	spencer.begin();
}

void loop(){
	LoopManager::loop();
}