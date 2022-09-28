#ifndef CIRCUITPET_LIBRARY_RGBLED_H
#define CIRCUITPET_LIBRARY_RGBLED_H
#include <Display/Color.h>

class RGBLed {
public:
	void begin();
	const Pixel& getColor() const;
	void setColor(Pixel color);
	uint8_t getBrightness() const;
	void setBrightness(uint8_t brightness);

private:
	Pixel color = {0, 0, 0};
	bool attached = false;

	uint8_t brightness = 200;
};


#endif //CIRCUITPET_LIBRARY_RGBLED_H
