#ifndef CHATTER_LIBRARY_CHATTER_H
#define CHATTER_LIBRARY_CHATTER_H

#include <Arduino.h>
#include <CircuitOS.h>
#include <Display/Display.h>
#include <Loop/LoopManager.h>
#include <Loop/LoopListener.h>
#include <Input/InputShift.h>
#include <Audio/Piezo.h>
#include <SPI.h>
#include "Pins.hpp"
#include "Battery/BatteryService.h"

class Chatter2Impl {
public:
	Chatter2Impl();

	/**
	 * Initializes display, input, Piezo, SPIFFS, battery service, and LoRa SPI
	 * @param backlight Whether to turn on backlight at end of initialization.
	 */
	void begin(bool backlight = true);

	/**
	 * Set backlight brightness. Value 0 is minimum brightness so screen is still on.
	 * @param brightness brightness value from 0 to 255, internally scaled to 51 - 255
	 */
	void setBrightness(uint8_t brightness);

	/**
	 * Fades in from OFF to configured brightness.
	 */
	void fadeIn();

	/**
	 * Fades out from configured brightness to OFF.
	 */
	void fadeOut();

	/**
	 * Whether backlight is ON.
	 * @return False if backlight is completely off, true otherwise.
	 */
	bool backlightPowered() const;

	/**
	 * Turns off backlight.
	 */
	void backlightOff();

	Display* getDisplay();
	Input* getInput();
	SPIClass& getSPILoRa();

private:
	Display* display;
	InputShift* input;

	SPIClass spiLoRa;

	bool pwmInited = false;
	void initPWM();
	void deinitPWM();

	/**
	 * Return backlight duty cycle for desired brightness.
	 * @param brightness 0-255
	 * @return duty cycle 0-255
	 */
	static uint8_t mapDuty(uint8_t brightness);
};

extern Chatter2Impl Chatter;
#endif
