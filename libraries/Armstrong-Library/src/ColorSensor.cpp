#include "ColorSensor.h"
#include <Loop/LoopManager.h>

void ColorSensor::begin(){
	if(!sensor.begin()) return;

	sensor.setConfiguration(VEML6040_IT_320MS | VEML6040_AF_AUTO | VEML6040_SD_ENABLE);

	scan();

	readTimer = 0;
	LoopManager::addListener(this);
}

void ColorSensor::end(){
	LoopManager::removeListener(this);
}

void ColorSensor::loop(uint micros){
	readTimer += micros;
	if(readTimer < ReadInterval) return;

	readTimer = 0;
	scan();
}

void ColorSensor::scan(){
	float r = (float) sensor.getRed() / (float) UINT16_MAX;
	float g = (float) sensor.getGreen() / (float) UINT16_MAX;
	float b = (float) sensor.getBlue() / (float) UINT16_MAX;

	b = std::min(1.0f, b * 2.0f);

	r = constrain(pow(r, 6.0f) * 50.0f, 0.0f, 1.0f);
	g = constrain(pow(g, 6.0f) * 50.0f, 0.0f, 1.0f);
	b = constrain(pow(b, 6.0f) * 50.0f, 0.0f, 1.0f);

	color = {
			(uint8_t) std::round(r * 255.0),
			(uint8_t) std::round(g * 255.0),
			(uint8_t) std::round(b * 255.0)
	};
}

Pixel ColorSensor::getColor() const{
	return color;
}

uint8_t ColorSensor::getR() const{
	return color.r;
}

uint8_t ColorSensor::getG() const{
	return color.g;
}

uint8_t ColorSensor::getB() const{
	return color.b;
}
