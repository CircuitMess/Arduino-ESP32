#ifndef CHATTER_LIBRARY_SETTINGS_H
#define CHATTER_LIBRARY_SETTINGS_H

#include <Arduino.h>

#define SHUTDOWN_STEPS 5

struct SettingsData {
	uint32_t shutdownTime = 2;
	uint8_t screenBrightness = 200;
	bool sound = true;
	uint8_t RGBbrightness = 200;
	bool hwTested = false;
};

class SettingsImpl {
public:
	bool begin();

	void store();

	SettingsData& get();

	void reset();

private:
	SettingsData data;
};

extern SettingsImpl Settings;

#endif //CHATTER_LIBRARY_SETTINGS_H
