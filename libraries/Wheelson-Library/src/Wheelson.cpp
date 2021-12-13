#include "Wheelson.h"

Nuvoton Nuvo(Wire);
WheelsonLED LED;
WheelsonMotor Motors;
BatteryService Battery;

WheelsonImpl Wheelson;

WheelsonImpl::WheelsonImpl() : display(160, 128, -1, -1){ }

void WheelsonImpl::begin(){
	if(psramFound()){
		Serial.printf("PSRAM init: %s, free: %d B\n", psramInit() ? "Yes" : "No", ESP.getFreePsram());
	}else{
		Serial.println("No PSRAM detected");
	}

	disableCore0WDT();
	disableCore1WDT();

	WiFi.mode(WIFI_OFF);
	btStop();

	if(!Nuvo.begin()){
		Serial.println("Nuvoton error");
		for(;;);
	}

	if(!SPIFFS.begin()){
		Serial.println("SPIFFS error");
		for(;;);
	}

	Settings.begin();

	LoopManager::addListener(&Nuvo.getI2C());
	LoopManager::addListener(&Battery);

	input = new WheelsonInput();
	input->preregisterButtons({ 0, 1, 2, 3, 4, 5 });
	LoopManager::addListener(input);

	Battery.begin();
	display.begin();
}


Display& WheelsonImpl::getDisplay(){
	return display;
}

Input* WheelsonImpl::getInput(){
	return input;
}
