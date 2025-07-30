#include "BatteryService.h"
#include <Loop/LoopManager.h>
#include <soc/efuse_reg.h>
#include <Util/HWRevision.h>
#include "../Pins.hpp"

BatteryService Battery;


uint16_t BatteryService::mapReading(uint16_t reading){
	if(HWRevision::get() == 1){
		return reading;
	}else{
		return map(reading, 720, 920, 3700, 4500);
	}
}

void BatteryService::loop(uint micros){
	measureMicros += micros;
	if(measureMicros >= (MeasureInterval * 1000000) / MeasureCount){
		measureMicros = 0;

		if(HWRevision::get() == 1){
			measureSum += esp_adc_cal_raw_to_voltage(analogRead(BATTERY_PIN), &calChars) * Factor;
		}else{
			measureSum += analogRead(BATTERY_PIN);
		}

		measureCounter++;
		if(measureCounter == MeasureCount){
			measureSum = measureSum / MeasureCount;
			voltage = measureSum;
			measureCounter = 0;
			measureSum = 0;
		}
	}
}

void BatteryService::begin(){
	LoopManager::addListener(this);
	pinMode(BATTERY_PIN, INPUT);

	if(HWRevision::get() == 1){
		// TODO: Check if this stays low during deep sleep
		pinMode(CALIB_EN, OUTPUT);
		digitalWrite(CALIB_EN, 0);
		pinMode(CALIB_READ, INPUT);

		analogSetAttenuation(ADC_2_5db);

		//Newer ESP32's used in HWRevision 1 (Wireless-tag WT32-S1) have calibration in efuse
		esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_2_5, ADC_WIDTH_BIT_12, 0, &calChars);

		calibrate();

		printf("calibration offset: %d\n", calibOffset);
	}

	for(int i = 0; i < 10; i++){
		if(HWRevision::get() == 1){
			measureSum += esp_adc_cal_raw_to_voltage(analogRead(BATTERY_PIN), &calChars) * Factor;
		}else{
			measureSum += analogRead(BATTERY_PIN);
		}
	}
	measureSum = measureSum / MeasureCount;
	voltage = measureSum;
	measureSum = 0;
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
	int16_t percentage = map(getVoltage(), 3700, 4500, 0, 100);
	if(percentage < 0){
		return 0;
	}else if(percentage > 100){
		return 100;
	}else{
		return percentage;
	}
}

uint16_t BatteryService::getVoltage() const{
	if(HWRevision::get() == 1){
		return voltage + getVoltOffset();
	}else{
		return mapReading(voltage) + getVoltOffset();
	}
}

int16_t BatteryService::getVoltOffset() const{
	if(HWRevision::get() == 1){
		return calibOffset;
	}else{
		uint32_t upper = REG_GET_FIELD(EFUSE_BLK3_RDATA3_REG, EFUSE_RD_ADC1_TP_HIGH);
		uint32_t lower = REG_GET_FIELD(EFUSE_BLK3_RDATA3_REG, EFUSE_RD_ADC1_TP_LOW);
		return (upper << 7) | lower;
	}
}

void BatteryService::calibrate(){
	digitalWrite(CALIB_EN, 1);
	delay(100);

	float sum = 0;
	for(int i = 0; i < MeasureCount; i++){
		sum += esp_adc_cal_raw_to_voltage(analogRead(CALIB_READ), &calChars) * Factor;
	}
	const uint16_t volt = round(sum / (float) MeasureCount);

	calibOffset = CalibRef - volt;

	printf("calib read: %d, expected %d\n", volt, CalibRef);

	digitalWrite(CALIB_EN, 0);
	delay(100);
}
