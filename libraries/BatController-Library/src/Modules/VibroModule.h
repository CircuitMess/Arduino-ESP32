#ifndef BATCONTROLLER_FIRMWARE_VIBROMODULE_H
#define BATCONTROLLER_FIRMWARE_VIBROMODULE_H


#include "Module.h"
#include <Devices/AW9523.h>

class VibroModule : public Module {
public:
	VibroModule();

	void setVibrating(bool on);
	void setLEDFill(uint8_t percentage);
	/**
	 * @param index LED index starting from bottom to top.
	 * @param value 0-255; 0 - off, 255 - brightest
	 */
	void setLED(uint8_t index, uint8_t value);

protected:
	void init() override;
	void transmission() override;

private:
	AW9523 aw9523;
	static constexpr uint8_t VibroPin = 8;
	static constexpr uint8_t NumLEDs = 15;
	static uint8_t mapPin(uint8_t led);

	bool vibro = false;
	uint8_t fillPercentage = 0;
	void setVibro();
	void setLEDs();

	bool push = false;
};


#endif //BATCONTROLLER_FIRMWARE_VIBROMODULE_H
