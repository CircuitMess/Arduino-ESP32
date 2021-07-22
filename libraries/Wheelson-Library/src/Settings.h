#ifndef WHEELSON_LIBRARY_SETTINGS_H
#define WHEELSON_LIBRARY_SETTINGS_H

#include <Arduino.h>

const uint8_t ShutdownMinutes[] = {0, 1, 5, 15, 30};

struct SettingsData {
	uint8_t shutdownTime = 1; //index of value in vector, values : {0, 1, 5, 15, 30}[min]
	uint speedMultiplier = 255; // speed * (speedMultiplier / 255)
	bool inputTested = false;
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

#endif //WHEELSON_LIBRARY_SETTINGS_H
