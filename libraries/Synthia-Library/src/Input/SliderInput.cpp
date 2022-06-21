#include "SliderInput.h"
#include "../Pins.hpp"
#include <Loop/LoopManager.h>
#include "../Settings.h"

SliderInput::SliderInput() : respLeft(POT_L, true), respRight(POT_L, true){

}

void SliderInput::begin(){
	pinMode(POT_L, INPUT);
	pinMode(POT_R, INPUT);

	uint16_t potValue;

	potValue = analogRead(POT_L);
	leftEMA_S = potValue;
	leftPotValue = 255 - constrain(map(potValue, Settings.get().calibration.sliderLeftDown, Settings.get().calibration.sliderLeftUp, 0, 255), 0, 255);

	potValue = analogRead(POT_R);
	rightEMA_S = potValue;
	rightPotValue = 255 - constrain(map(potValue, Settings.get().calibration.sliderRightDown, Settings.get().calibration.sliderRightUp, 0, 255), 0, 255);

	valsLeft.push(leftEMA_S);
	valsRight.push(rightEMA_S);

	LoopManager::addListener(this);
}

uint8_t SliderInput::getLeftPotValue() const{
	return leftPotValue;
}

uint8_t SliderInput::getRightPotValue() const{
	return rightPotValue;
}

void SliderInput::loop(uint time){
	valsLeft.push(leftFilter(analogRead(POT_L)));
	valsRight.push(rightFilter(analogRead(POT_R)));

	if(valsLeft.size() < valsLeft.getMax()) return;

	respLeft.update(valsLeft.getAverage());
	respRight.update(valsRight.getAverage());

	int16_t left = respLeft.getValue();
	int16_t right = respRight.getValue();

	if(leftPotRawPreviousValue == -1){
		leftPotRawPreviousValue = left;
	}else if(abs(leftPotRawPreviousValue - left) < 4){
		left = leftPotRawPreviousValue;
	}

	if(rightPotRawPreviousValue == -1){
		rightPotRawPreviousValue = right;
	}else if(abs(rightPotRawPreviousValue - right) < 4){
		right = rightPotRawPreviousValue;
	}

	uint16_t cLeftPotValue = 255 - constrain(map(left, Settings.get().calibration.sliderLeftDown, Settings.get().calibration.sliderLeftUp, 0, 255), 0, 255);
	uint16_t cRightPotValue = 255 - constrain(map(right, Settings.get().calibration.sliderRightDown, Settings.get().calibration.sliderRightUp, 0, 255), 0, 255);

	if(leftPotPreviousValue == -1){
		leftPotPreviousValue = leftPotValue = cLeftPotValue;
	}
	if(abs((int) cLeftPotValue - (int) leftPotPreviousValue) >= MinPotMove){
		leftPotPreviousValue = leftPotValue = cLeftPotValue;
		iterateListeners([this](SliderListener* l){
			l->leftPotMove(leftPotValue);
		});

		if(leftPotCallback){
			leftPotCallback(leftPotValue);
		}
	}

	if(rightPotPreviousValue == -1){
		rightPotPreviousValue = rightPotValue = cRightPotValue;
	}
	if(abs((int) cRightPotValue - (int) rightPotPreviousValue) >= MinPotMove){
		rightPotPreviousValue = rightPotValue = cRightPotValue;
		iterateListeners([this](SliderListener* l){
			l->rightPotMove(rightPotValue);
		});

		if(rightPotCallback){
			rightPotCallback(rightPotValue);
		}
	}
}

uint16_t SliderInput::leftFilter(uint16_t sensorValue){
	leftEMA_S = (EMA_a * (float) sensorValue) + ((1.0f - EMA_a) * leftEMA_S);
	return max(0.0, round(leftEMA_S));
}

uint16_t SliderInput::rightFilter(uint16_t sensorValue){
	rightEMA_S = (EMA_a * (float) sensorValue) + ((1.0f - EMA_a) * rightEMA_S);
	return max(0.0, round(rightEMA_S));
}

void SliderInput::setLeftPotCallback(void (* leftPotCallback)(uint8_t)){
	SliderInput::leftPotCallback = leftPotCallback;
}

void SliderInput::setRightPotCallback(void (* rightPotCallback)(uint8_t)){
	SliderInput::rightPotCallback = rightPotCallback;
}

void SliderListener::rightPotMove(uint8_t value){ }

void SliderListener::leftPotMove(uint8_t value){ }

