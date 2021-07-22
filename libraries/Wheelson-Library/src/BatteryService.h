#ifndef WHEELSON_LIBRARY_BATTERYSERVICE_H
#define WHEELSON_LIBRARY_BATTERYSERVICE_H

#include <Arduino.h>
#include <Loop/LoopListener.h>
#include <Wire.h>
#include <Display/Sprite.h>

class BatteryService : public LoopListener {
public:
	BatteryService();

	virtual ~BatteryService();

	void loop(uint micros) override;
	uint16_t getVoltage() const;
	uint8_t getLevel();
	uint8_t getPercentage();

//	bool isCharging() const;

	void disableShutdown(bool _shutdown);

	uint8_t getLastDrawnLevel() const;

	void drawIcon(Sprite* canvas);

	void begin();

private:
	uint16_t voltage = 0; //in mV
	static const uint16_t measureInterval;
	uint measureMicros = measureInterval*1000000;
	uint8_t lastDrawn=0;
	bool shutdownDisable = false;
	Color* batteryBuffer[4] = {};
	uint8_t level = 0;
	void uploadBuffer(int i);
//	bool charging = false;
};


#endif //WHEELSON_LIBRARY_BATTERYSERVICE_H
