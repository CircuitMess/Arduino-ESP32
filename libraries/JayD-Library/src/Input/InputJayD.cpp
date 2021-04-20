#include "InputJayD.h"
#include "../JayD.h"
#include <Wire.h>
#include <driver/i2s.h>


InputJayD *InputJayD::instance;

InputJayD::InputJayD() : btnPressCallbacks(NUM_BTN, nullptr), btnReleaseCallbacks(NUM_BTN, nullptr),
						 btnHoldCallbacks(NUM_BTN, nullptr),
						 encMovedCallbacks(NUM_ENC, nullptr), potMovedCallbacks(NUM_POT, nullptr),
						 btnHoldValue(NUM_BTN, 0), btnHoldStart(NUM_BTN, 0),
						 wasPressed(NUM_BTN, false){

	Wire.begin(I2C_SDA, I2C_SCL);

	instance = this;

}

void InputJayD::reset(){
	digitalWrite(JDNV_PIN_RESET, LOW);
	delay(5);
	digitalWrite(JDNV_PIN_RESET, HIGH);

}

bool InputJayD::identify(){
	Wire.beginTransmission(JDNV_ADDR);
	Wire.write(BYTE_IDENTIFY);
	Wire.endTransmission();
	Wire.requestFrom(JDNV_ADDR, 1);
	if(Wire.available()){
		if(Wire.read() == JDNV_ADDR){
			return true;
		}
		else{
			return false;
		}
	}
	else{
		return false;
	}
}

bool InputJayD::begin(){
	pinMode(JDNV_PIN_RESET, OUTPUT);
	digitalWrite(JDNV_PIN_RESET, HIGH);
	reset();
	delay(10);
	Wire.beginTransmission(JDNV_ADDR);

	if(Wire.endTransmission() != 0){
		return false;
	}

	return identify();

}

void InputJayD::setBtnPressCallback(uint8_t id, void (*callback)()){
	btnPressCallbacks[id] = callback;
}

void InputJayD::setBtnReleaseCallback(uint8_t id, void (*callback)()){
	btnReleaseCallbacks[id] = callback;
}

void InputJayD::removeBtnPressCallback(uint8_t id){
	btnPressCallbacks[id] = nullptr;
}

void InputJayD::removeBtnReleaseCallback(uint8_t id){
	btnReleaseCallbacks[id] = nullptr;
}

InputJayD *InputJayD::getInstance(){
	return instance;
}

void InputJayD::setBtnHeldCallback(uint8_t id, uint32_t holdTime, void (*callback)()){
	btnHoldCallbacks[id] = callback;
	btnHoldValue[id] = holdTime;
}

void InputJayD::removeBtnHeldCallback(uint8_t id){
	btnHoldCallbacks[id] = nullptr;
}


void InputJayD::setEncoderMovedCallback(uint8_t id, void (*callback)(int8_t value)){
	encMovedCallbacks[id] = callback;
}

void InputJayD::removeEncoderMovedCallback(uint8_t id){
	encMovedCallbacks[id] = nullptr;
}

void InputJayD::setPotMovedCallback(uint8_t id, void (*callback)(uint8_t value)){
	potMovedCallbacks[id] = callback;
}

void InputJayD::removePotMovedCallback(uint8_t id){
	potMovedCallbacks[id] = nullptr;
}

void InputJayD::addListener(JayDInputListener* listener){
	listeners.push_back(listener);
}

void InputJayD::removeListener(JayDInputListener* listener){
	int index = listeners.indexOf(listener);
	if(index == (uint) -1) return;
	listeners.remove(index);
}

uint8_t InputJayD::getNumEvents(){
	Wire.beginTransmission(JDNV_ADDR);
	Wire.write(BYTE_NUMEVENTS);
	Wire.endTransmission();
	Wire.requestFrom(JDNV_ADDR, 2);
	if(Wire.available()){
		Wire.read();//addr
	}
	if(Wire.available()){
		uint8_t numEventsData = Wire.read();
		return numEventsData;
	}
}

uint8_t InputJayD::getPotValue(uint8_t potID){
	Wire.beginTransmission(JDNV_ADDR);
	Wire.write(BYTE_GETPOTVALUE);
	Wire.write(potID);
	Wire.endTransmission();
	Wire.requestFrom(JDNV_ADDR, 2);
	if(Wire.available()){
		Wire.read();//addr
	}
	if(Wire.available()){
		uint8_t potValue = Wire.read();
		return potValue;
	}
}


void InputJayD::fetchEvents(int numEvents){
	if(numEvents == 0) return;
	Wire.beginTransmission(JDNV_ADDR);
	Wire.write(BYTE_GETEVENTS);
	Wire.write(numEvents);
	Wire.endTransmission();
	Wire.requestFrom(JDNV_ADDR, 2 * numEvents + 1);
	if(Wire.available()){
		Wire.read();
	}
	std::vector<InputEvent> events;
	uint8_t deviceId;
	int8_t valueData;
	for(int i = numEvents; i > 0; i--){
		if(Wire.available()){
			deviceId = Wire.read();
		}
		if(Wire.available()){
			valueData = Wire.read();

		}
		uint8_t id = deviceId & 0x0F;
		uint8_t device = deviceId >> 4;

		events.push_back({(DeviceType) device, id, valueData});
	}
	for(InputEvent event:events){
		if(event.deviceType == BTN){
			handleButtonEvent(event.deviceID, event.value);
		}else if(event.deviceType == ENC){
			handleEncoderEvent(event.deviceID, event.value);
		}else if(event.deviceType == POT){
			handlePotentiometerEvent(event.deviceID, event.value);
		}
	}
}

void InputJayD::handleButtonEvent(uint8_t id, uint8_t value){
	if(value == 1){
		btnHoldStart[id] = millis();
		if(btnPressCallbacks[id] != nullptr){
			btnPressCallbacks[id]();
		}

		for(auto& listener : listeners){
			listener->buttonPress(id);
		}
	}

	if(value == 0){
		btnHoldStart[id] = 0;
		wasPressed[id] = false;

		if(btnReleaseCallbacks[id] != nullptr){
			btnReleaseCallbacks[id]();
		}

		for(auto& listener : listeners){
			listener->buttonRelease(id);
		}
	}
}

void InputJayD::buttonHoldCheck(){
	for(uint8_t i = 0; i < btnHoldCallbacks.size(); i++){
		if(btnHoldStart[i] == 0) continue;
		if(millis() - btnHoldStart[i] >= btnHoldValue[i] && !wasPressed[i]){
			wasPressed[i] = true;

			if(btnHoldCallbacks[i] != nullptr){
				btnHoldCallbacks[i]();
			}

			for(auto& listener : listeners){
				listener->buttonHold(i);
			}
		}
	}
}

void InputJayD::handleEncoderEvent(uint8_t id, int8_t value){
	if(encoderTime == 0){
		encoderTime = currentTime;
	}
	tempEncValue[id] += value;
}

void InputJayD::handlePotentiometerEvent(uint8_t id, uint8_t value){
	value = constrain(((float) value / 235.0f) * 255.0f, 0, 255);
	if(potMovedCallbacks[id] != nullptr){
		potMovedCallbacks[id](value);
	}

	for(auto& listener : listeners){
		listener->potMove(id, value);
	}
}

void InputJayD::loop(uint _time){
	fetchEvents(getNumEvents());
	buttonHoldCheck();

	currentTime = millis();
	if(currentTime - encoderTime >= holdTime){
		for(int i = 0; i < 7; i++){
			if(tempEncValue[i] != 0){
				if(encMovedCallbacks[i]){
					encMovedCallbacks[i](tempEncValue[i]);
				}

				for(auto& listener : listeners){
					listener->encoderMove(i, tempEncValue[i]);
				}

				tempEncValue[i] = 0;
			}
		}
		encoderTime = currentTime;
	}
}

void InputJayD::setHoldTime(uint32_t holdTime){
	InputJayD::holdTime = holdTime;
}

void JayDInputListener::buttonPress(uint8_t id){ }

void JayDInputListener::buttonRelease(uint8_t id){ }

void JayDInputListener::buttonHold(uint8_t id){ }

void JayDInputListener::encoderMove(uint8_t id, int8_t value){ }

void JayDInputListener::potMove(uint8_t id, uint8_t value){ }
