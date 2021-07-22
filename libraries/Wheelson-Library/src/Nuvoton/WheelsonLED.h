#ifndef WHEELSON_LIBRARY_WHEELSONLED_H
#define WHEELSON_LIBRARY_WHEELSONLED_H

#include <Wire.h>
#include <Sync/Mutex.h>
#include "I2CQueue.h"

#define BACKLIGHT_SET_BYTE 0x20
#define BACKLIGHT_GET_BYTE 0x21
#define HEADLIGHT_SET_BYTE 0x22
#define HEADLIGHT_GET_BYTE 0x23
#define RGB_SET_BYTE 0x24
#define RGB_GET_BYTE 0x25

enum WLEDColor {
		OFF, RED, GREEN, YELLOW, BLUE, MAGENTA, CYAN, WHITE
};

class WheelsonLED {
public:
	WheelsonLED();
	void setBacklight(bool backlight);
	bool getBacklight();
	void setHeadlight(uint8_t intensity);
	uint8_t getHeadlight();
	void setRGB(WLEDColor colour);
	WLEDColor getRGB();

private:
	I2CQueue& i2c;

};


#endif //WHEELSON_LIBRARY_WHEELSONLED_H
