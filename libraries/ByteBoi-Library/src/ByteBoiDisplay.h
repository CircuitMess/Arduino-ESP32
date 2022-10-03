#ifndef BYTEBOI_LIBRARY_BYTEBOIDISPLAY_H
#define BYTEBOI_LIBRARY_BYTEBOIDISPLAY_H

#include <Arduino.h>

#define LGFX_USE_V1
#define ARDUINO_LOLIN_D32_PRO

#include <LovyanGFX.hpp>

class ByteBoiDisplay : public lgfx::LGFX_Device {
public:

	lgfx::Panel_ILI9341* panel1() const;
	lgfx::Panel_ST7789* panel2() const;

};


#endif //BYTEBOI_LIBRARY_BYTEBOIDISPLAY_H
