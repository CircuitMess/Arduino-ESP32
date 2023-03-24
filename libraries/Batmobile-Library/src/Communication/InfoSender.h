#ifndef BATMOBILE_LIBRARY_INFOSENDER_H
#define BATMOBILE_LIBRARY_INFOSENDER_H

#include <Loop/LoopListener.h>

class InfoSender : private LoopListener {
public:
	void begin();
	~InfoSender() override;

private:
	void loop(uint micros) override;

	uint32_t signalTime = 0, batteryTime = 0;
	static constexpr uint32_t SignalInterval = 2000000;
	static constexpr uint32_t BatteryInterval = 2000000;

	uint8_t getSignalStrength();
	static constexpr int8_t MaxSS = -50;
	static constexpr int8_t MinSS = -90;

};


#endif //BATMOBILE_LIBRARY_INFOSENDER_H
