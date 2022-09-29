#include "BatteryService.h"
#include <Loop/LoopManager.h>
#include <soc/efuse_reg.h>
#include "../Pins.hpp"
#include "../CircuitPet.h"

BatteryService Battery;

uint16_t BatteryService::mapReading(uint16_t reading){
	int mapped = map(reading, 1640, 1960, MIN_VOLT, MAX_VOLT);
	return mapped;
}

void BatteryService::begin(){
	LoopManager::addListener(this);
	pinMode(PIN_CHARGE, INPUT_PULLDOWN);
	pinMode(PIN_BATT, INPUT);

	for(int i = 0; i < MeasureCount; i++){
		measureVoltage += analogRead(PIN_BATT);
	}

	voltage = mapReading(measureVoltage / MeasureCount);

	measureVoltage = 0;
	measureCount = 0;
	measureTime = millis();

	if(getVoltage() < MIN_VOLT && !charging()){
		CircuitPet.shutdown();
		return;
	}
}

void BatteryService::loop(uint micros){
	if(millis() - measureTime <= 1000.0f * MeasureInverval / MeasureCount) return;

	measureVoltage += analogRead(PIN_BATT);
	measureTime = millis();

	if(++measureCount < MeasureCount) return;

	voltage = mapReading(measureVoltage / MeasureCount);
	measureVoltage = 0;
	measureCount = 0;

	if(getVoltage() < MIN_VOLT && !charging()){
		CircuitPet.shutdown();
		return;
	}
}

uint8_t BatteryService::getLevel() const{
	uint8_t percentage = getPercentage();
	if(percentage > 80){
		return 5;
	}else if(percentage > 60){
		return 4;
	}else if(percentage > 40){
		return 3;
	}else if(percentage > 20){
		return 2;
	}else if(percentage >= 5){
		return 1;
	}else if(percentage < 5){
		return 0;
	}
}

uint8_t BatteryService::getPercentage() const{
	int16_t percentage = map(getVoltage(), MIN_VOLT, MAX_VOLT, 0, 100);
	if(percentage < 0){
		return 0;
	}else if(percentage > 100){
		return 100;
	}else{
		return percentage;
	}
}

uint16_t BatteryService::getVoltage() const{
	return voltage + getVoltOffset();
}

int16_t BatteryService::getVoltOffset(){
	uint32_t upper = REG_GET_FIELD(EFUSE_BLK3_RDATA3_REG, EFUSE_RD_ADC1_TP_HIGH);
	uint32_t lower = REG_GET_FIELD(EFUSE_BLK3_RDATA3_REG, EFUSE_RD_ADC1_TP_LOW);
	return (upper << 7) | lower;
}

bool BatteryService::charging() const{
	return digitalRead(PIN_CHARGE);
}