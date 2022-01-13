#include "BatteryService.h"
#include "../ByteBoi.h"
#include "../Bitmaps/battery_0.hpp"
#include "../Bitmaps/battery_1.hpp"
#include "../Bitmaps/battery_2.hpp"
#include "../Bitmaps/battery_3.hpp"
#include "../Bitmaps/battery_4.hpp"
#include <SPIFFS.h>
#include <Loop/LoopManager.h>

const uint16_t BatteryService::measureInterval = 2; //in seconds
const uint16_t BatteryService::measureCount = 10; //in seconds

void BatteryService::loop(uint micros){
	measureMicros += micros;
	if(measureMicros >= (measureInterval * 1000000) / measureCount){
		measureMicros = 0;
		measureSum += analogRead(BATTERY_PIN);
		measureCounter++;
		if(measureCounter == measureCount){
			measureSum = measureSum / measureCount;
			voltage = (1.1 * measureSum + 683);
			measureCounter = 0;
			measureSum = 0;

			if(getLevel() == 0 && autoShutdown && !chargePinDetected()){
				ByteBoi.shutdown();
				return;
			}
		}
		measureMicros = 0;
	}
}

uint8_t BatteryService::getLevel() const{
	uint8_t percentage = getPercentage();
	if(percentage > 80){
		return 4;
	}else if(percentage <= 80 && percentage > 40){
		return 3;
	}else if(percentage <= 40 && percentage > 15){
		return 2;
	}else if(percentage <= 15 && percentage > 0){
		return 1;
	}else if(percentage == 0){
		return 0;
	}
}

uint16_t BatteryService::getVoltage() const{
	if(chargePinDetected()){
		return ((float)voltage - (2289.61 - 0.523723*(float)voltage));
	}else{
		return voltage;
	}
}

uint8_t BatteryService::getPercentage() const{
	int16_t percentage = map(getVoltage(), 3650, 4250, 0, 100);
	if(percentage < 0){
		return 0;
	}else if(percentage > 100){
		return 100;
	}else{
		return percentage;
	}
}

void BatteryService::setAutoShutdown(bool enabled){
	autoShutdown = enabled;
}

void BatteryService::begin(){
	LoopManager::addListener(this);
	ByteBoi.getExpander()->pinMode(CHARGE_DETECT_PIN, INPUT_PULLDOWN);
	pinMode(BATTERY_PIN, INPUT);
	for(int i = 0; i < 5; i++){
		batteryBuffer[i] = static_cast<Color*>(malloc(sizeof(batteryIcon_4)));
	}
	memcpy_P(batteryBuffer[0],batteryIcon_0,sizeof(batteryIcon_0));
	memcpy_P(batteryBuffer[1],batteryIcon_1,sizeof(batteryIcon_1));
	memcpy_P(batteryBuffer[2],batteryIcon_2,sizeof(batteryIcon_2));
	memcpy_P(batteryBuffer[3],batteryIcon_3,sizeof(batteryIcon_3));
	memcpy_P(batteryBuffer[4],batteryIcon_4,sizeof(batteryIcon_4));
}

bool BatteryService::chargePinDetected() const{
	return ByteBoi.getExpander()->getPortState() & (1 << CHARGE_DETECT_PIN);
}

bool BatteryService::isCharging() const{
	if(getLevel() == 4){
		return false;
	}else{
		return chargePinDetected();
	}
}

void BatteryService::drawIcon(Sprite &sprite, int16_t x, int16_t y, int16_t level){
	if(level != -1){
		if(level > 4 || level < 0) return;
		sprite.drawIcon(batteryBuffer[level], x, y, 14, 6, 1, TFT_TRANSPARENT);
		return;
	}

	Color* buffer = batteryBuffer[getLevel()];
	if(buffer == nullptr) return;
	if(!isCharging() && timePassed != 0){
		timePassed = 0;
	}
	if(isCharging()){
		if(timePassed == 0){
			timePassed = millis();
			pictureIndex = 0;
		}
		if(millis() - timePassed >= 300){
			timePassed = millis();
			pictureIndex++;
			if(pictureIndex > 4){
				pictureIndex = 0;
			}
		}
		sprite.drawIcon(batteryBuffer[pictureIndex], x, y, 14, 6, 1, TFT_TRANSPARENT);
	}else{
		sprite.drawIcon(buffer, x, y, 14, 6, 1, TFT_TRANSPARENT);
	}
}
