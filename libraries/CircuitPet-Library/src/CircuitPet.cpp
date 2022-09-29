#include "CircuitPet.h"
#include <Loop/LoopManager.h>
#include "Pins.hpp"
#include <SPIFFS.h>
#include "Settings.h"
#include <Audio/Piezo.h>
#include <ctime>
#include "Battery/BatteryService.h"

CircuitPetImpl CircuitPet;
ChirpSystem Audio;
RGBController RGB;

CircuitPetImpl::CircuitPetImpl() : display(160, 128, -1, -3), rtc(I2C_BM8563_DEFAULT_ADDRESS, Wire){

}

void CircuitPetImpl::begin(bool backlight){

	analogSetAttenuation(ADC_6db);

	SPIFFS.begin();

	display.getTft()->setPanel(CircuitPetDisplay::panel1());
	display.begin();
	display.getTft()->setRotation(3);
	display.swapBytes(false);
	display.clear(TFT_BLACK);
	display.commit();

	Settings.begin();

	input.preregisterButtons({ BTN_A, BTN_B, BTN_LEFT, BTN_RIGHT });
	LoopManager::addListener(&input);

	led.begin();
	led.setColor(Pixel::Black);
	led.setBrightness(Settings.get().RGBbrightness);
	RGB.begin(&led);

	Piezo.begin(PIN_BUZZ);
	Piezo.noTone();

	pinMode(PIN_BL, OUTPUT);
	digitalWrite(PIN_BL, 1);

	// Init RTC
	Wire.begin(I2C_SDA, I2C_SCL);
	Wire.setClock(100000);
	rtc.begin();

	//check rtc time, if outside unixtime or invalid, set to unixtime 0
	I2C_BM8563_DateTypeDef dateStruct;
	rtc.getDate(&dateStruct);
	if(dateStruct.year < 1970 || dateStruct.year >= 2038 ||
	   dateStruct.date < 1 || dateStruct.date > 31 ||
	   dateStruct.month < 1 || dateStruct.month > 12){
		setUnixTime(0);
	}


	Battery.begin();

	if(backlight){
		fadeIn();
	}
}

Input* CircuitPetImpl::getInput(){
	return &input;
}

Display* CircuitPetImpl::getDisplay(){
	return &display;
}

void CircuitPetImpl::initPWM(){
	ledcSetup(6, 5000, 8);
	ledcAttachPin(PIN_BL, 6);
	pwmInited = true;
}

void CircuitPetImpl::deinitPWM(){
	ledcDetachPin(PIN_BL);
	digitalWrite(PIN_BL, HIGH);
	pwmInited = false;
}

void CircuitPetImpl::setBrightness(uint8_t brightness){
	if(!pwmInited){
		initPWM();
	}

	ledcWrite(6, mapDuty(brightness));
}

bool CircuitPetImpl::backlightPowered() const{
	return pwmInited;
}

void CircuitPetImpl::backlightOff(){
	deinitPWM();
}

uint8_t CircuitPetImpl::mapDuty(uint8_t brightness){
	return map(brightness, 0, 255, 240, 0);
}

void CircuitPetImpl::fadeOut(uint8_t d){
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

void CircuitPetImpl::fadeIn(uint8_t d){
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

void CircuitPetImpl::setUnixTime(time_t unixtime){
	std::tm* t = std::localtime(&unixtime);
	I2C_BM8563_DateTypeDef dateStruct { (int8_t)t->tm_wday, (int8_t)(t->tm_mon + 1), (int8_t)t->tm_mday, (int16_t)(t->tm_year + 1900) };
	I2C_BM8563_TimeTypeDef timeStruct { (int8_t)t->tm_hour, (int8_t)t->tm_min, (int8_t)t->tm_sec };

	rtc.setDate(&dateStruct);
	rtc.setTime(&timeStruct);
}

time_t CircuitPetImpl::getUnixTime(){

	std::time_t timestampCandidates[RTCRedundancy];
	I2C_BM8563_DateTypeDef dateStruct;
	I2C_BM8563_TimeTypeDef timeStruct;

	for(uint8_t i = 0; i < RTCRedundancy; i++){
		// Get RTC
		rtc.getDate(&dateStruct);
		rtc.getTime(&timeStruct);

		std::tm t { timeStruct.seconds,
					timeStruct.minutes,
					timeStruct.hours,
					dateStruct.date,
					dateStruct.month - 1,
					dateStruct.year - 1900};

		timestampCandidates[i] = mktime(&t);
	}

	uint8_t votes[RTCRedundancy] = {0};
	for(int i = 0; i < RTCRedundancy; ++i){
		for(int j = 0; j < RTCRedundancy; ++j){
			if(i == j) continue;
			if(abs(difftime(timestampCandidates[i], timestampCandidates[j])) <= 2.0f){
				votes[i]++;
			}
		}
	}

	return timestampCandidates[*std::max_element(votes, votes+RTCRedundancy)];
}

void CircuitPetImpl::shutdown(){
	RGB.setColor(Pixel::Red);
	delay(350);
	RGB.setColor(Pixel::Black);
	delay(700);
	RGB.setColor(Pixel::Red);
	delay(350);
	RGB.setColor(Pixel::Black);

	ledcDetachPin(PIN_BUZZ);
	ledcDetachPin(RGB_R);
	ledcDetachPin(RGB_G);
	ledcDetachPin(RGB_B);
	esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
	esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
	esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
	esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
	esp_deep_sleep_start();
}

void CircuitPetImpl::sleep(){
	Piezo.noTone();
	RGB.setColor(Pixel::Black);

	esp_sleep_enable_gpio_wakeup();
	gpio_wakeup_enable((gpio_num_t) BTN_LEFT, GPIO_INTR_LOW_LEVEL);
	gpio_wakeup_enable((gpio_num_t) BTN_RIGHT, GPIO_INTR_LOW_LEVEL);
	gpio_wakeup_enable((gpio_num_t) BTN_ENTER, GPIO_INTR_LOW_LEVEL);
	gpio_wakeup_enable((gpio_num_t) BTN_BACK, GPIO_INTR_LOW_LEVEL);

	auto perc = std::max(Battery.getPercentage(), (uint8_t) 10);
	esp_sleep_enable_timer_wakeup((172800000000 / 100) * perc); // 2 days

	fadeOut();
	esp_light_sleep_start();

	auto cause = esp_sleep_get_wakeup_cause();
	if(cause == ESP_SLEEP_WAKEUP_TIMER){
		shutdown();
		return;
	}

	fadeIn();
}

const I2C_BM8563& CircuitPetImpl::getRTC() const{
	return rtc;
}
