#include "Chatter.h"
#include <Loop/LoopManager.h>
#include "Pins.hpp"
#include "Battery/BatteryService.h"
#include "Settings.h"
#include <SPIFFS.h>

ChatterImpl Chatter;

ChatterImpl::ChatterImpl() : spiLoRa(HSPI){}

void ChatterImpl::begin(bool backlight){
	Serial.begin(115200);

	pinMode(PIN_BL, OUTPUT);
	digitalWrite(PIN_BL, 1);

	Piezo.begin(PIN_BUZZ);
	Piezo.noTone();

	display = new Display(160, 128, -1, 3);
	display->begin();
	display->getTft()->setRotation(1);
	display->swapBytes(false);
	display->getBaseSprite()->clear(TFT_BLACK);
	display->commit();

	if(!SPIFFS.begin()){
		Serial.println("SPIFFS failed");
	}

	Settings.begin();

	input = new InputShift(INPUT_DATA, INPUT_CLOCK, INPUT_LOAD, 16);
	input->begin();

	Vector<uint8_t> buttons;
	for(int i = 0; i < 16; i++){
		buttons.push_back(i);
	}
	input->preregisterButtons(buttons);

	LoopManager::addListener(input);

	spiLoRa.begin(RADIO_SCK, RADIO_MISO, RADIO_MOSI, RADIO_CS);

	Battery.begin();

	if(backlight){
		fadeIn();
	}
}

void ChatterImpl::setBrightness(uint8_t brightness){
	if(!pwmInited){
		initPWM();
	}

	ledcWrite(6, mapDuty(brightness));
}

Display* ChatterImpl::getDisplay(){
	return display;
}

Input* ChatterImpl::getInput(){
	return input;
}

SPIClass &ChatterImpl::getSPILoRa(){
	return spiLoRa;
}

void ChatterImpl::fadeOut(){
	if(!pwmInited){
		initPWM();
	}

	uint8_t dutyOn = mapDuty(Settings.get().screenBrightness);

	for(int i = 0; i <= 255; i++){
		uint8_t val = map(i, 0, 255, dutyOn, 255);
		ledcWrite(6, val);
		delay(2);
	}

	deinitPWM();
}

void ChatterImpl::fadeIn(){
	if(!pwmInited){
		initPWM();
	}

	uint8_t dutyOn = mapDuty(Settings.get().screenBrightness);

	for(int i = 0; i <= 255; i++){
		uint8_t val = map(i, 0, 255, 255, dutyOn);
		ledcWrite(6, val);
		delay(2);
	}
}

void ChatterImpl::initPWM(){
	ledcSetup(6, 5000, 8);
	ledcAttachPin(PIN_BL, 6);
	pwmInited = true;
}

void ChatterImpl::deinitPWM(){
	ledcDetachPin(PIN_BL);
	digitalWrite(PIN_BL, HIGH);
	pwmInited = false;
}

bool ChatterImpl::backlightPowered() const{
	return pwmInited;
}

void ChatterImpl::backlightOff(){
	deinitPWM();
}

uint8_t ChatterImpl::mapDuty(uint8_t brightness){
	return map(brightness, 0, 255, 240, 0);
}
