#ifndef ARMSTRONG_LIBRARY_RGBCONTROLLER_H
#define ARMSTRONG_LIBRARY_RGBCONTROLLER_H

#include "Pins.hpp"
#include <cstdint>
#include <Display/Color.h>

class RGBController {
public:
	void begin();
	void end();

	void set(Pixel pixel);
	void set(uint8_t r, uint8_t g, uint8_t b);

	void setR(uint8_t val);
	void setG(uint8_t val);
	void setB(uint8_t val);

	uint8_t getR();
	uint8_t getG();
	uint8_t getB();

	void set(uint8_t i, uint8_t val);
	uint8_t get(uint8_t i);

private:
	const uint8_t Pins[3] = { LED_R, LED_G, LED_B };
	const uint8_t PWMChannels[3] = { 4, 5, 6 };

	uint8_t state[3] = {0};
};


#endif //ARMSTRONG_LIBRARY_RGBCONTROLLER_H
