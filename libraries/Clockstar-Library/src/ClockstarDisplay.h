#ifndef CLOCKSTAR_LIBRARY_CIRCUITPETDISPLAY_H
#define CLOCKSTAR_LIBRARY_CIRCUITPETDISPLAY_H

#include <Arduino.h>

#define LGFX_USE_V1
#define ARDUINO_LOLIN_D32_PRO

#include <LovyanGFX.hpp>

class ClockstarDisplay : public lgfx::LGFX_Device {
public:
	static lgfx::Panel_ST7735S* panel1();
};

#endif //CLOCKSTAR_LIBRARY_CIRCUITPETDISPLAY_H
