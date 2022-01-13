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
	ByteBoi.getExpander()->pinMode(LED_R, OUTPUT);
	ByteBoi.getExpander()->pinMode(LED_G, OUTPUT);
	ByteBoi.getExpander()->pinMode(LED_B, OUTPUT);
}

void ByteBoiLED::setRGB(LEDColor colour){
	if(!Settings.get().RGBenable && colour != OFF) return;

	currentColor = colour;
	uint8_t color = rgbMap[(uint8_t)colour];
	ByteBoi.getExpander()->pinWrite(LED_R, (color & 0b100));
	ByteBoi.getExpander()->pinWrite(LED_G, (color & 0b010));
	ByteBoi.getExpander()->pinWrite(LED_B, (color & 0b001));
}
LEDColor ByteBoiLED::getRGB(){
	return currentColor;
}


