#ifndef CIRCUITPET_LIBRARY_CIRCUITPET_H
#define CIRCUITPET_LIBRARY_CIRCUITPET_H

#include <Arduino.h>
#include <CircuitOS.h>
#include <Wire.h>
#include <Loop/LoopListener.h>
#include <Loop/LoopManager.h>
#include <Display/Display.h>
#include <Input/InputGPIO.h>
#include <Audio/ChirpSystem.h>
#include <Audio/Piezo.h>
#include <I2C_BM8563.h>
#include "Settings.h"
#include "RGBLed.h"
#include "RGBController.h"
#include "CircuitPetDisplay.h"
#include "RGBController.h"
#include "Battery/BatteryService.h"
#include "Pins.hpp"

class CircuitPetImpl {
public:
	CircuitPetImpl();

	void begin(bool backlight = true);

	Input* getInput();
	Display* getDisplay();

	/**
	 * Set backlight brightness. Value 0 is minimum brightness so screen is still on.
	 * @param brightness brightness value from 0 to 255, internally scaled to 51 - 255
	 */
	void setBrightness(uint8_t brightness);

	/**
	 * Fades in from OFF to configured brightness.
	 */
	void fadeIn(uint8_t delay = 2);

	/**
	 * Fades out from configured brightness to OFF.
	 */
	void fadeOut(uint8_t delay = 2);

	/**
	 * Whether backlight is ON.
	 * @return False if backlight is completely off, true otherwise.
	 */
	bool backlightPowered() const;

	/**
	 * Turns off backlight.
	 */
	void backlightOff();

	time_t getUnixTime();
	void setUnixTime(time_t unixtime);

	void shutdown();
	void sleep();

	const I2C_BM8563& getRTC() const;

private:
	Display display;
	InputGPIO input;
	I2C_BM8563 rtc;
	RGBLed led;

	bool powerCutFlag = false;

	bool pwmInited = false;
	void initPWM();
	void deinitPWM();

	/**
	 * Return backlight duty cycle for desired brightness.
	 * @param brightness 0-255
	 * @return duty cycle 0-255
	 */
	static uint8_t mapDuty(uint8_t brightness);

	constexpr static uint8_t RTCRedundancy = 5;
};

extern CircuitPetImpl CircuitPet;
extern ChirpSystem Audio;
extern RGBController RGB;


#endif //CIRCUITPET_LIBRARY_CIRCUITPET_H
