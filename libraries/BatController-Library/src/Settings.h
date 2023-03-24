#ifndef BATCTRL_LIBRARY_SETTINGS_H
#define BATCTRL_LIBRARY_SETTINGS_H

#include <Arduino.h>

#define SHUTDOWN_STEPS 5

extern const uint32_t ShutdownSeconds[SHUTDOWN_STEPS];
extern const char* ShutdownText[SHUTDOWN_STEPS];

struct SettingsData {
	uint8_t soundVolume = 100;
	uint32_t shutdownTime = 2;
	uint8_t screenBrightness = 180;
	bool hwTested = false;
	char ssid[24] = {0};
	char password[23] = {0};
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

#endif //BATCTRL_LIBRARY_SETTINGS_H
