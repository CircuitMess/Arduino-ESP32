#ifndef CIRCUITPET_FIRMWARE_RGBCONTROLLER_H
#define CIRCUITPET_FIRMWARE_RGBCONTROLLER_H

#include <Arduino.h>
#include <Loop/LoopListener.h>
#include <Display/Color.h>
#include "RGBLed.h"

class RGBController : private LoopListener {
public:
	RGBController();
	void begin(RGBLed* led);

	void setColor(Pixel color = { 0, 0, 0});
	void clear();
	void setSolid(Pixel color);
	void blink(Pixel color);
	void blinkTwice(Pixel color);
	void blinkContinuous(Pixel color);

	void loop(uint micros) final;

	void setBrightness(uint8_t brightness);
	uint8_t getBrightness() const;

private:
	RGBLed* led = nullptr;

	enum {
		Solid, Once, Twice, Continuous
	} slotStates =  Solid;

	Pixel slotColors =  Pixel::Black;

	Pixel blinkColors =  Pixel::Black;
	uint32_t blinkStartTimes =  0;
	bool blinkStates =  false;
	const uint32_t blinkDuration = 100; //[ms]
	const uint32_t blinkContinuousDuration = 500; //[ms]
};

#endif //CIRCUITPET_FIRMWARE_RGBCONTROLLER_H
