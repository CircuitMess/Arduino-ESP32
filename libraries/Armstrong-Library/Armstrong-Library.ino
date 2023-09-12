#include <Arduino.h>
#include <CircuitOS.h>
#include "src/Armstrong.h"
#include <Loop/LoopManager.h>

void setup(){
	Serial.begin(115200);
	Armstrong.begin();
}

void loop(){
	LoopManager::loop();
}
