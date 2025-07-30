#include "BatteryService.h"
#include "../ByteBoi.h"
#include "../Bitmaps/battery_0.hpp"
#include "../Bitmaps/battery_1.hpp"
#include "../Bitmaps/battery_2.hpp"
#include "../Bitmaps/battery_3.hpp"
#include "../Bitmaps/battery_4.hpp"
#include <SPIFFS.h>
#include <Loop/LoopManager.h>
#include <Util/HWRevision.h>

void BatteryService::loop(uint micros){
	measureMicros += micros;
	if(measureMicros >= (measureInterval * 1000000) / measureCount){
		measureMicros = 0;

		if(hasChars){
			measureSum += esp_adc_cal_raw_to_voltage(analogRead(BATTERY_PIN), &calChars) * Factor;
		}else{
			measureSum += analogRead(BATTERY_PIN);
		}
		measureCounter++;
		if(measureCounter == measureCount){
			voltage = (int) round(measureSum / measureCount);

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

uint16_t BatteryService::getVoltage(bool bypassChrg) const{
	if(chargePinDetected() && !bypassChrg){
		return 5000;
	}

	if(ByteBoi.getVer() == ByteBoiImpl::v2_0){
		if(hasChars){
			return voltage;
		}else{
			return (int) round(1.0683 * voltage - 197.0);
		}
	}else if(ByteBoi.getVer() == ByteBoiImpl::v1_1){
		return (int) round(0.587 * voltage + 1694.0);
	}else{ // v1.0
		return (int) round(1.1 * voltage + 683);
	}
}

uint8_t BatteryService::getPercentage() const{
	int16_t percentage;

	if(ByteBoi.getVer() == ByteBoiImpl::v2_0){
		percentage = map(getVoltage(), 3650, 4100, 0, 100);
	}else if(ByteBoi.getVer() == ByteBoiImpl::v1_1){
		percentage = map(getVoltage(), 3650, 4000, 0, 100);
	}else{
		percentage = map(getVoltage(), 3650, 4250, 0, 100);
	}

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

	pinMode(BATTERY_PIN, INPUT);
	for(int i = 0; i < 5; i++){
		batteryBuffer[i] = static_cast<Color*>(malloc(sizeof(batteryIcon_4)));
	}
	memcpy_P(batteryBuffer[0],batteryIcon_0,sizeof(batteryIcon_0));
	memcpy_P(batteryBuffer[1],batteryIcon_1,sizeof(batteryIcon_1));
	memcpy_P(batteryBuffer[2],batteryIcon_2,sizeof(batteryIcon_2));
	memcpy_P(batteryBuffer[3],batteryIcon_3,sizeof(batteryIcon_3));
	memcpy_P(batteryBuffer[4],batteryIcon_4,sizeof(batteryIcon_4));

	if(ByteBoi.getVer() == ByteBoiImpl::Ver::v1_0){
		ByteBoi.getExpander()->pinMode(CHARGE_DETECT_PIN, INPUT_PULLDOWN);
	}else if(ByteBoi.getVer() == ByteBoiImpl::Ver::v1_1 || ByteBoi.getVer() == ByteBoiImpl::Ver::v2_0){
		pinMode(CHARGE_DETECT_PIN, INPUT_PULLDOWN);

		if(ByteBoi.getVer() == ByteBoiImpl::Ver::v2_0){
			// TODO: Check if this stays low during deep sleep
			pinMode(CALIB_EN, OUTPUT);
			digitalWrite(CALIB_EN, 0);

			analogSetAttenuation(ADC_2_5db);

			if(esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF) == ESP_OK || esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_TP) == ESP_OK){
				if(esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_2_5, ADC_WIDTH_BIT_12, 0, &calChars) != ESP_ADC_CAL_VAL_DEFAULT_VREF){
					hasChars = true;
				}
			}

			if(!hasChars){
				printf("No ADC calib in efuse found!\n");
			}

			//NOTE: Design error on HW v2.3, GPIO35 is input-only and cannot be used here
			// calibrate();
		}else if(ByteBoi.getVer() == ByteBoiImpl::v1_1){
			analogSetAttenuation(ADC_11db);
		}
	}

	for(int i = 0; i < measureCount; i++){
		if(hasChars){
			measureSum += esp_adc_cal_raw_to_voltage(analogRead(BATTERY_PIN), &calChars) * Factor;
		}else{
			measureSum += analogRead(BATTERY_PIN);
		}
	}
	voltage = (int) round(measureSum / measureCount);
	measureSum = 0;
}

bool BatteryService::chargePinDetected() const{
	auto expander = ByteBoi.getExpander();
	if(expander){
		return expander->getPortState() & (1 << CHARGE_DETECT_PIN);
	}else{
		return digitalRead(CHARGE_DETECT_PIN) == HIGH;
	}
}

bool BatteryService::isCharging() const{
	return chargePinDetected();
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

void BatteryService::calibrate(){
	digitalWrite(CALIB_EN, 1);
	delay(100);

	float sum = 0;
	for(int i = 0; i < measureCount; i++){
		sum += analogRead(BATTERY_PIN);
		delay(100 / measureCount);
	}
	const uint16_t volt = round(sum / (float) measureCount);

	calibOffset = CalibRef - volt;

	digitalWrite(CALIB_EN, 0);
	delay(100);
}
