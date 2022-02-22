#ifndef CHATTER_LIBRARY_BATTERYSERVICE_H
#define CHATTER_LIBRARY_BATTERYSERVICE_H

#include <Arduino.h>
#include <Loop/LoopListener.h>

class BatteryService : public LoopListener {
public:
	BatteryService()= default;

	void loop(uint micros) override;
	void begin();
	uint8_t getLevel() const;
	uint8_t getPercentage() const;
	uint16_t getVoltage() const;

private:
	uint16_t voltage = 0; //in mV
	static const uint16_t MeasureInterval;
	static const uint16_t MeasureCount;
	uint measureMicros = 0;
	float measureSum = 0;
	uint8_t measureCounter = 0;


};

extern BatteryService Battery;

#endif //CHATTER_LIBRARY_BATTERYSERVICE_H
