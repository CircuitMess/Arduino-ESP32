#include "ByteBoiLED.h"
#include "ByteBoi.h"
#include "Settings.h"
ByteBoiLED LED;

const uint8_t rgbMap[] = {
		0b000,
		0b100,
		0b010,
		0b001,
		0b110,
		0b101,
		0b011,
		0b111
};

void ByteBoiLED::begin(){
	auto expander = ByteBoi.getExpander();

	if(expander){
		expander->pinMode(LED_R, OUTPUT);
		expander->pinMode(LED_G, OUTPUT);
		expander->pinMode(LED_B, OUTPUT);
	}else{
		pinMode(LED_R, OUTPUT);
		pinMode(LED_G, OUTPUT);
		pinMode(LED_B, OUTPUT);
	}
}

void ByteBoiLED::setRGB(LEDColor colour){
	if(!Settings.get().RGBenable && colour != OFF) return;

	currentColor = colour;
	uint8_t color = rgbMap[(uint8_t)colour];

	auto expander = ByteBoi.getExpander();
	if(expander){
		expander->pinWrite(LED_R, (color & 0b100));
		expander->pinWrite(LED_G, (color & 0b010));
		expander->pinWrite(LED_B, (color & 0b001));
	}else{
		digitalWrite(LED_R, (color & 0b100) ? HIGH : LOW);
		digitalWrite(LED_G, (color & 0b010) ? HIGH : LOW);
		digitalWrite(LED_B, (color & 0b001) ? HIGH : LOW);
	}
}
LEDColor ByteBoiLED::getRGB(){
	return currentColor;
}


