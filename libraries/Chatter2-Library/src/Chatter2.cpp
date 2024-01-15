#include "Chatter2.h"
#include <Loop/LoopManager.h>
#include "Pins.hpp"
#include "Battery/BatteryService.h"
#include "Settings.h"
#include <SPIFFS.h>

Chatter2Impl Chatter;

Chatter2Impl::Chatter2Impl() : spiLoRa(HSPI){}

void Chatter2Impl::begin(bool backlight){
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

void Chatter2Impl::setBrightness(uint8_t brightness){
	if(!pwmInited){
		initPWM();
	}

	ledcWrite(6, mapDuty(brightness));
}

Display* Chatter2Impl::getDisplay(){
	return display;
}

Input* Chatter2Impl::getInput(){
	return input;
}

SPIClass &Chatter2Impl::getSPILoRa(){
	return spiLoRa;
}

void Chatter2Impl::fadeOut(){
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

	display->getBaseSprite()->clear(TFT_BLACK);
	display->commit();
}

void Chatter2Impl::fadeIn(){
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

void Chatter2Impl::initPWM(){
	ledcSetup(6, 5000, 8);
	ledcAttachPin(PIN_BL, 6);
	pwmInited = true;
}

void Chatter2Impl::deinitPWM(){
	ledcDetachPin(PIN_BL);
	digitalWrite(PIN_BL, HIGH);
	pwmInited = false;
}

bool Chatter2Impl::backlightPowered() const{
	return pwmInited;
}

void Chatter2Impl::backlightOff(){
	deinitPWM();
}

uint8_t Chatter2Impl::mapDuty(uint8_t brightness){
	return map(brightness, 0, 255, 240, 0);
}
