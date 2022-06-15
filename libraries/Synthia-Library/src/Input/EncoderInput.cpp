#include <Loop/LoopManager.h>
#include "EncoderInput.h"
#include "../Pins.hpp"

EncoderInput::EncoderInput(){

}

void EncoderInput::begin(){
	pinMode(ENC_L1, INPUT);
	pinMode(ENC_L2, INPUT);
	pinMode(ENC_R1, INPUT);
	pinMode(ENC_R2, INPUT);

	LoopManager::addListener(this);
}

void EncoderInput::loop(uint time){
	int8_t leftVal = 0;
	int8_t rightVal = 0;

	int stateLeft = digitalRead(ENC_L1);
	if(stateLeft != prevStateLeft && prevStateLeft != INT32_MAX){
		if(digitalRead(ENC_L2) != stateLeft){
			leftVal = 1;
		}else{
			leftVal = -1;
		}
	}
	prevStateLeft = stateLeft;

	int stateRight = digitalRead(ENC_R1);
	if(stateRight != prevStateRight && prevStateRight != INT32_MAX){
		if(digitalRead(ENC_R2) != stateRight){
			rightVal = 1;
		}else{
			rightVal = -1;
		}
	}
	prevStateRight = stateRight;

	if(leftVal != 0){
		iterateListeners([&leftVal](EncoderListener* l){
			l->leftEncMove(leftVal);
		});

		if(leftEncCallback){
			leftEncCallback(leftVal);
		}
	}

	if(rightVal != 0){
		iterateListeners([&rightVal](EncoderListener* l){
			l->rightEncMove(rightVal);
		});

		if(rightEncCallback){
			rightEncCallback(rightVal);
		}
	}
}

void EncoderInput::setLeftEncCallback(void (* leftEncListener)(int8_t)){
	EncoderInput::leftEncCallback = leftEncListener;
}

void EncoderInput::setRightEncCallback(void (* rightEncListener)(int8_t)){
	EncoderInput::rightEncCallback = rightEncListener;
}

void EncoderListener::leftEncMove(int8_t amount){ }

void EncoderListener::rightEncMove(int8_t amount){ }

