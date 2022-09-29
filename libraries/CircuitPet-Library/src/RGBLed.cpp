#include "RGBLed.h"
#include "Pins.hpp"
#include <Arduino.h>

void RGBLed::begin(){
	pinMode(RGB_R, OUTPUT);
	pinMode(RGB_G, OUTPUT);
	pinMode(RGB_B, OUTPUT);
	digitalWrite(RGB_R, HIGH);
	digitalWrite(RGB_G, HIGH);
	digitalWrite(RGB_B, HIGH);

	ledcSetup(4, 1000, 8);
	ledcSetup(2, 1000, 8);
	ledcSetup(3, 1000, 8);

	ledcAttachPin(RGB_R, 4);
	ledcAttachPin(RGB_G, 2);
	ledcAttachPin(RGB_B, 3);
	attached = true;
}

const Pixel& RGBLed::getColor() const{
	return color;
}

void RGBLed::setColor(Pixel color){
	double brightness = pow((double) this->brightness / 255.0, 2.0);
	color.r = floor((float) color.r * brightness);
	color.g = floor((float) color.g * brightness);
	color.b = floor((float) color.b * brightness);

	if(color == Pixel::Black){
		ledcDetachPin(RGB_R);
		ledcDetachPin(RGB_G);
		ledcDetachPin(RGB_B);

		pinMode(RGB_R, OUTPUT);
		pinMode(RGB_G, OUTPUT);
		pinMode(RGB_B, OUTPUT);

		digitalWrite(RGB_R, HIGH);
		digitalWrite(RGB_G, HIGH);
		digitalWrite(RGB_B, HIGH);

		attached = false;
	}else if(!attached){
		attached = true;
		ledcAttachPin(RGB_R, 4);
		ledcAttachPin(RGB_G, 2);
		ledcAttachPin(RGB_B, 3);

		ledcSetup(4, 1000, 8);
		ledcSetup(2, 1000, 8);
		ledcSetup(3, 1000, 8);
	}
	RGBLed::color = color;

	ledcWrite(4, (255 - color.r));
	ledcWrite(2, (255 - color.g));
	ledcWrite(3, (255 - color.b));
}

uint8_t RGBLed::getBrightness() const{
	return brightness;
}

void RGBLed::setBrightness(uint8_t brightness){
	RGBLed::brightness = brightness;
}
