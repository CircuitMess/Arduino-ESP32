#include <Arduino.h>
#include <CircuitOS.h>
#include <BatCommon.h>
#include "src/BatController.h"

void setup(){
    Serial.begin(115200);
    BatController.begin(true);

	BatController.getDisplay()->clear(TFT_RED);
	BatController.getDisplay()->commit();
}

void loop(){

}