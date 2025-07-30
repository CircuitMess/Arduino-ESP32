#ifndef CHATTER_LIBRARY_BATTERYSERVICE_H
#define CHATTER_LIBRARY_BATTERYSERVICE_H

#include <Arduino.h>
#include <Loop/LoopListener.h>
#include <esp_adc_cal.h>

class BatteryService : public LoopListener {
public:
	BatteryService()= default;

	void loop(uint micros) override;
	void begin();
	uint8_t getLevel() const;
	uint8_t getPercentage() const;
	uint16_t getVoltage() const;

	int16_t getVoltOffset() const;

private:
	uint16_t voltage = 0; //in mV
	static constexpr uint16_t MeasureInterval = 10; // [s]
	static constexpr uint16_t MeasureCount = 10;
	uint measureMicros = 0;
	float measureSum = 0;
	uint8_t measureCounter = 0;

	static uint16_t mapReading(uint16_t reading);

	//For HW revision 2.0
	void calibrate();

	static constexpr uint16_t CalibRef = 2500;
	static constexpr uint16_t Factor = 4;
	int16_t calibOffset;

	esp_adc_cal_characteristics_t calChars;
};

extern BatteryService Battery;

#endif //CHATTER_LIBRARY_BATTERYSERVICE_H
