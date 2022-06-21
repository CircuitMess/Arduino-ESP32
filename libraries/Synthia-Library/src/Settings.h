#ifndef SYNTHIA_LIBRARY_SETTINGS_H
#define SYNTHIA_LIBRARY_SETTINGS_H

#include <Arduino.h>

//contains the raw readings for the sliders' actual range
struct SlidersCalibration {
	uint16_t sliderLeftUp = 690;
	uint16_t sliderRightUp = 690;
	uint16_t sliderLeftDown = 220;
	uint16_t sliderRightDown = 220;
};

struct SettingsData {
	bool tested = false;
	SlidersCalibration calibration;
	uint8_t brightness = 255;
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

#endif //SYNTHIA_LIBRARY_SETTINGS_H
