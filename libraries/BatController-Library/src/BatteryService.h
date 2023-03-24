#ifndef BATCONTROLLER_LIBRARY_BATTERYSERVICE_H
#define BATCONTROLLER_LIBRARY_BATTERYSERVICE_H

#include <Arduino.h>
#include <Loop/LoopListener.h>

#define MAX_VOLT 4200
#define MIN_VOLT 3600


class BatteryService : private LoopListener {
public:
	BatteryService() = default;

	void loop(uint micros) override;
	void begin();
	uint8_t getLevel() const;
	uint8_t getPercentage() const;
	uint16_t getVoltage() const;

	static uint16_t mapReading(uint16_t reading);

	static int16_t getVoltOffset();

	bool charging() const;

	static uint8_t percToLevel(uint8_t percentage);

private:
	static constexpr float MeasureInterval = 2;
	static constexpr uint8_t MeasureCount = 30;

	uint32_t measureVoltage = 0;
	uint8_t measureCount = 0;
	uint32_t measureTime = 0;

	uint16_t voltage = 0;

};

extern BatteryService Battery;

#endif //BATCONTROLLER_LIBRARY_BATTERYSERVICE_H
