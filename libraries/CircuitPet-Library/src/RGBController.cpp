#include "RGBController.h"
#include <Devices/Matrix/Matrix.h>
#include <Loop/LoopManager.h>
#include <SPIFFS.h>
#include <FS/RamFile.h>

RGBController::RGBController(){

}

void RGBController::begin(RGBLed* led){
	this->led = led;
	LoopManager::addListener(this);
}

void RGBController::setColor(Pixel color){
	slotColors = color;

	if(slotStates == Solid){
		led->setColor(color);
	}
}

void RGBController::clear(){
	slotStates = Solid;
	blinkStates = false;
	blinkColors = Pixel::Black;
	blinkStartTimes = 0;

	slotColors = Pixel::Black;
	led->setColor(Pixel::Black);
}

void RGBController::setSolid(Pixel color){

	slotStates = Solid;

	setColor(color);
}

void RGBController::blink(Pixel color){
	slotStates = Once;
	blinkColors = color;
	blinkStartTimes = millis();
	blinkStates = true;

	led->setColor(color);
}

void RGBController::blinkTwice(Pixel color){
	blink(color);
	slotStates = Twice;
}

void RGBController::blinkContinuous(Pixel color){
	blink(color);
	slotStates = Continuous;
}

void RGBController::loop(uint micros){
	if(slotStates == Solid) return;
	if(millis() - blinkStartTimes < blinkDuration) return;

	if(slotStates == Continuous){
		if(millis() - blinkStartTimes < blinkContinuousDuration) return;

		blinkStates = !blinkStates;
		blinkStartTimes = millis();
		led->setColor(blinkStates ? blinkColors : Pixel::Black);
	}else if(slotStates == Twice && blinkStates){
		blinkStates = false;
		blinkStartTimes = millis();
		led->setColor(Pixel::Black);
	}else if(slotStates == Twice && !blinkStates){
		blinkStates = true;
		blinkStartTimes = millis();
		slotStates = Once;
		led->setColor(blinkColors);
	}else if(slotStates == Once){
		blinkStates = false;
		blinkStartTimes = 0;
		blinkColors = Pixel::Black;
		slotStates = Solid;
		led->setColor(slotColors);
	}
}

void RGBController::setBrightness(uint8_t brightness){
	led->setBrightness(brightness);
}

uint8_t RGBController::getBrightness() const{
	return led->getBrightness();
}
