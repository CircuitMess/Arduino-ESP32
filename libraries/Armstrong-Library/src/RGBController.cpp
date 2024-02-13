#include "RGBController.h"
#include <Wire.h>

void RGBController::begin(){
	for(int i = 0; i < 3; i++){
		ledcSetup(PWMChannels[i], 1000, 8);
		ledcWrite(PWMChannels[i], 255);
		ledcAttachPin(Pins[i], PWMChannels[i]);

		state[i] = 0;
	}
}

void RGBController::end(){
	for(int i = 0; i < 3; i++){
		ledcDetachPin(Pins[i]);
		pinMode(Pins[i], OUTPUT);
		digitalWrite(Pins[i], HIGH);
	}
}

void RGBController::set(uint8_t r, uint8_t g, uint8_t b){
	set(0, r);
	set(1, g);
	set(2, b);
}

void RGBController::set(Pixel pixel){
	set(pixel.r, pixel.g, pixel.b);
}

uint8_t RGBController::getR(){
	return get(0);
}

uint8_t RGBController::getG(){
	return get(1);
}

uint8_t RGBController::getB(){
	return get(2);
}

void RGBController::setR(uint8_t val){
	set(0, val);
}

void RGBController::setG(uint8_t val){
	set(1, val);
}

void RGBController::setB(uint8_t val){
	set(2, val);
}

void RGBController::set(uint8_t i, uint8_t val){
	if(i >= 3) return;

	state[i] = val;
	const auto adjusted = (uint8_t) round(255.0f - pow((float) val / 255.0f, 2.0f) * 255.0f);
	ledcWrite(PWMChannels[i], adjusted);
}

uint8_t RGBController::get(uint8_t i){
	if(i >= 3) return 0;
	return state[i];
}
