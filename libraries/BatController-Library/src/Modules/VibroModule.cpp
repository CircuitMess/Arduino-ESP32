#include "VibroModule.h"


VibroModule::VibroModule() : Module(0x59), aw9523(::Wire, 0x59){}

void VibroModule::init(){
	if(!aw9523.begin()){
		errorOccured();
		return;
	}

	vibro = false;

	for(int i = 0; i < 16; ++i){
		if(i == VibroPin){
			aw9523.pinMode(VibroPin, AW9523::OUT);
			setVibro();
		}else{
			aw9523.pinMode(i, AW9523::LED);
			aw9523.dim(i, 0);
		}
	}
	push = false;
	fillPercentage = 0;
}

void VibroModule::transmission(){
	if(push){
		setVibro();
		setLEDs();
		push = false;
	}
}

void VibroModule::setVibrating(bool on){
	vibro = on;
	push = true;
}

void VibroModule::setLEDFill(uint8_t percentage){
	if(fillPercentage == percentage) return;

	fillPercentage = percentage;
	push = true;
}

void VibroModule::setVibro(){
	aw9523.write(VibroPin, !vibro);
}

void VibroModule::setLEDs(){
	uint32_t fill = map(fillPercentage, 0, 100, 0, NumLEDs * 255);
	for(int i = 0; i < NumLEDs; ++i){
		uint8_t pixelValue = min(fill, 255U);
		float fp = (float) pixelValue / 255.0f;
		fp *= 0.2f;
		fp = pow(fp, 2);
		pixelValue = fp * 255.0f;
		aw9523.dim(mapPin(i), pixelValue);
		if(fill >= 255){
			fill -= 255;
		}else if(fill > 0){
			fill = 0;
		}
	}
}

uint8_t VibroModule::mapPin(uint8_t led){
	if(led <= 3) return 15 - led;
	else if(led <= 11) return 11 - led;
	else return 23 - led;
}

void VibroModule::setLED(uint8_t index, uint8_t value){
	float fp = (float) value / 255.0f;
	fp *= 0.2f;
	fp = pow(fp, 2);
	value = fp * 255.0f;
	aw9523.dim(mapPin(index), value);
	push = true;
}
//pins bottom to top: 15, 14, 13, 12, 7, 6, 5, 4, 3, 2, 1, 0, 11, 10, 9
