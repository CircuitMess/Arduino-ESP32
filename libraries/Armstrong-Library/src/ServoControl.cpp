#include <Loop/LoopManager.h>
#include "ServoControl.h"

ServoControl::ServoControl(){
	for(const auto& pair : Pins){
		state.insert({ pair.first, StartPos.find(pair.first)->second });
		truePos.insert({ pair.first, StartPos.find(pair.first)->second });
	}
}

void ServoControl::begin(){
	for(const auto& pair : Pins){
		const auto motor = pair.first;
		const auto pin = pair.second;
		const auto chan = PWMChannels.find(motor)->second;

		state[motor] = StartPos.find(motor)->second;
		truePos[motor] = StartPos.find(motor)->second;

		ledcSetup(chan, 200, 8);
		ledcWrite(chan, mapToRange(motor, state[motor]));
		ledcAttachPin(pin, chan);
	}

	if(easing){
		LoopManager::addListener(this);
	}
}

void ServoControl::end(){
	for(const auto& pair : Pins){
		const auto motor = pair.first;
		const auto pin = pair.second;
		const auto chan = PWMChannels.find(motor)->second;

		ledcDetachPin(chan);
		pinMode(pin, INPUT);
	}

	LoopManager::removeListener(this);
}

void ServoControl::sendPos(Motor motor, uint8_t pos){
	truePos[motor] = pos;
	auto val = mapToRange(motor, pos);
	auto chan = PWMChannels.find(motor)->second;
	ledcWrite(chan, val);
}

void ServoControl::setPos(Motor motor, uint8_t pos){
	auto pair = state.find(motor);
	if(pair == state.end()) return;

	state[motor] = pos;

	if(!easing){
		sendPos(motor, pos);
	}
}

uint8_t ServoControl::getPos(Motor motor) const{
	auto pair = state.find(motor);
	if(pair == state.end()) return 0;
	return pair->second;
}

void ServoControl::centerPos(){
	for(const auto& pair : state){
		const auto motor = pair.first;
		const auto pos = StartPos.find(motor)->second;

		setPos(motor, pos);
	}
}

uint8_t ServoControl::mapToRange(Motor motor, uint8_t value) const{
	const auto pair = Limits.find(motor);
	if(pair == Limits.end()) return UINT8_MAX / 2;

	const auto lim = pair->second;
	return map(value, 0, 255, lim.min, lim.max);
}

void ServoControl::enableEasing(bool enabled){
	easing = enabled;

	if(enabled){
		LoopManager::addListener(this);
	}else{
		LoopManager::removeListener(this);

		for(int i = 0; i < 4; i++){
			auto motor = (Motor) i;
			auto pos = truePos[motor];
			auto target = state[motor];

			if(pos != target){
				sendPos(motor, target);
			}
		}
	}
}

bool ServoControl::easingEnabled() const{
	return easing;
}

void ServoControl::loop(uint micros){
	if(!easing){
		LoopManager::removeListener(this);
		return;
	}

	easeTime += micros;
	if(easeTime < EaseInterval) return;
	easeTime = 0;

	for(int i = 0; i < 4; i++){
		auto motor = (Motor) i;
		auto pos = truePos[motor];
		auto target = state[motor];
		if(pos == target) continue;

		int8_t dir = 0;
		if(target > pos){
			dir = 1;
		}else if(target < pos){
			dir = -1;
		}

		sendPos(motor, pos + dir);
	}
}

void ServoControl::setEasingInterval(uint8_t interval){
	if(interval == 0){
		interval = 1;
	}

	EaseInterval = interval * 1000;
}
