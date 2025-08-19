#include "Wheelson.h"
#include "PinDef.h"
#include <Util/HWRevision.h>

Nuvoton Nuvo(Wire);
WheelsonLED LED;
WheelsonMotor Motors;
BatteryService Battery;
PinMap<Pin> Pins;

WheelsonImpl Wheelson;

WheelsonImpl::WheelsonImpl() : display(160, 128, -1, -1){}

void WheelsonImpl::begin(){
	initVer();

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
		// for(;;);
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

	auto sprite = (TFT_eSprite*) display.getBaseSprite();
	sprite->deleteSprite();
	sprite->setPsram(true);
	sprite->createSprite(160, 128);

	if(ver == Ver::v3 || ver == Ver::v2){
		display.getTft()->setPanel(WheelsonDisplay::panel2());
	}else{
		display.getTft()->setPanel(WheelsonDisplay::panel1());
	}
	display.begin();
}


Display& WheelsonImpl::getDisplay(){
	return display;
}

Input* WheelsonImpl::getInput(){
	return input;
}

void WheelsonImpl::initVer(int override){
	if(verInited) return;
	verInited = true;

	const auto hw = (override == -1) ? HWRevision::get() : override;

	if(hw == 2){
		printf("ver3\n");
		ver = Ver::v3;
		Pins.set(Pins3);
	}else if(hw == 1){
		printf("ver2\n");
		ver = Ver::v2;
		Pins.set(Pins12);
	}else if(hw == 0){
		printf("ver1\n");
		ver = Ver::v1;
		Pins.set(Pins12);
	}
}
