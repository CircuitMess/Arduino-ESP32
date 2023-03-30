#include "BatController.h"
#include <Loop/LoopManager.h>
#include <SPIFFS.h>
#include <Wire.h>
#include "Settings.h"
#include "BatteryService.h"
#include <WiFi.h>
#include <esp_wifi.h>

BatControllerImpl BatController;
BatteryService Battery;
Communication Com;
VibroModule vibro;
AcceleroModule accelero;

BatControllerImpl::BatControllerImpl() : display(160, 128, -1, -3){
}

void BatControllerImpl::begin(bool backlight) {
	pinMode(PIN_BL, OUTPUT);
	digitalWrite(PIN_BL, HIGH);

	Wire.begin(I2C_SDA, I2C_SCL);
	SPIFFS.begin();
	Settings.begin();

	display.getTft()->setPanel(BatControllerDisplay::panel1());
    display.begin();
    display.getTft()->setRotation(1);
	display.clear(TFT_BLACK);
    display.commit();

	Battery.begin();

	input.preregisterButtons({ BTN_UP, BTN_DOWN, BTN_LEFT, BTN_RIGHT, BTN_A, BTN_B, BTN_C });
	LoopManager::addListener(&input);

	accelero.begin();
	vibro.begin();

    if(backlight){
        fadeIn();
    }
}

Display* BatControllerImpl::getDisplay(){
	return &display;
}

Input* BatControllerImpl::getInput(){
	return &input;
}



void BatControllerImpl::setBrightness(uint8_t brightness){
    if(!pwmInited){
        initPWM();
    }

    ledcWrite(6, mapDuty(brightness));
}

bool BatControllerImpl::backlightPowered() const{
    return pwmInited;
}

void BatControllerImpl::backlightOff(){
    deinitPWM();
}


void BatControllerImpl::fadeOut(uint8_t d){
    if(!pwmInited){
        initPWM();
    }

	uint8_t dutyOn = mapDuty(Settings.get().screenBrightness);

	for(int i = 0; i <= 255; i++){
		uint8_t val = map(i, 0, 255, dutyOn, 255);
		ledcWrite(6, val);
		delay(d);
	}

    deinitPWM();
}

void BatControllerImpl::fadeIn(uint8_t d){
    if(!pwmInited){
        initPWM();
    }

	uint8_t dutyOn = mapDuty(Settings.get().screenBrightness);

	for(int i = 0; i <= 255; i++){
		uint8_t val = map(i, 0, 255, 255, dutyOn);
		ledcWrite(6, val);
		delay(d);
	}
}

void BatControllerImpl::initPWM() {
    ledcSetup(6, 5000, 8);
    ledcAttachPin(PIN_BL, 6);
    pwmInited = true;
}

void BatControllerImpl::deinitPWM() {
    ledcDetachPin(PIN_BL);
	pinMode(PIN_BL, OUTPUT);
    digitalWrite(PIN_BL, HIGH);
    pwmInited = false;
}

uint8_t BatControllerImpl::mapDuty(uint8_t brightness) {
	return map(brightness, 0, 255, 240, 0);
}

void BatControllerImpl::shutdown() {
    if(backlightPowered()){
       fadeOut();
    }

    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
	esp_wifi_stop();
	adc_power_off();

    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    esp_deep_sleep_start();
}
