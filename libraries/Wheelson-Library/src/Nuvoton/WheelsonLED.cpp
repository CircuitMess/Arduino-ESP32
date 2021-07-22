#include "WheelsonLED.h"
#include "Nuvoton.h"
#include "../Wheelson.h"

WheelsonLED::WheelsonLED() : i2c(Nuvo.getI2C()){

}

void WheelsonLED::setBacklight(bool backlight) {
	uint8_t msg[] = { BACKLIGHT_SET_BYTE, backlight };
	i2c.send(msg, 2);
}

bool WheelsonLED::getBacklight() {
	uint8_t msg[] = { BACKLIGHT_GET_BYTE };
	uint8_t value;
	i2c.request(msg, 1, &value, 1);
	return value;
}

void WheelsonLED::setHeadlight(uint8_t headlight) {
	uint8_t msg[] = { HEADLIGHT_SET_BYTE, headlight };
	i2c.send(msg, 2);
}

uint8_t WheelsonLED::getHeadlight() {
	uint8_t msg[] = { HEADLIGHT_GET_BYTE };
	uint8_t value;
	i2c.request(msg, 1, &value, 1);
	return value & 0xFF;
}

void WheelsonLED::setRGB(WLEDColor colour){
	uint8_t msg[] = { RGB_SET_BYTE, static_cast<uint8_t>(colour) };
	i2c.send(msg, 2);
}

WLEDColor WheelsonLED::getRGB(){
	uint8_t msg[] = { RGB_GET_BYTE };
	uint8_t value;
	i2c.request(msg, 1, &value, 1);

	uint8_t bitColour = value & 0xFF;
	return (WLEDColor)bitColour;
}
