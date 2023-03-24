#ifndef BATCONTROLLER_LIBRARY_BATCONTROLLER_H
#define BATCONTROLLER_LIBRARY_BATCONTROLLER_H

#include <Arduino.h>
#include <CircuitOS.h>
#include <Display/Display.h>
#include <Input/InputGPIO.h>
#include "Pins.hpp"
#include "BatControllerDisplay.h"
#include "BatteryService.h"
#include "Com/Communication.h"
#include "Modules/VibroModule.h"
#include "Modules/AcceleroModule.h"

class BatControllerImpl {
public:
	BatControllerImpl();

	void begin(bool backlight = true);

	Display* getDisplay();
    Input* getInput();

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

    void shutdown();

private:
	Display display;
	InputGPIO input;

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

extern BatControllerImpl BatController;
extern BatteryService Battery;
extern Communication Com;
extern VibroModule vibro;
extern AcceleroModule accelero;

#endif //BATCONTROLLER_LIBRARY_BATCONTROLLER_H
