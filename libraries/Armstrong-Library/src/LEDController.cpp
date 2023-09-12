#include "LEDController.h"

LEDController::LEDController() : output(SHIFTOUT_CLOCK, SHIFTOUT_DATA){
	for(const auto& pair : Pins){
		state.insert({ pair.first, false });
	}
}

void LEDController::begin(){
	output.begin();

	for(const auto& pair : Pins){
		state[pair.first] = false;
		output.set(pair.second, false);
	}
}

void LEDController::end(){
	clear();
}

void LEDController::set(Slot slot, bool value){
	auto pair = Pins.find(slot);
	if(pair == Pins.end()) return;

	output.set(pair->second, value);
	state[slot] = value;
}

bool LEDController::get(Slot slot) const{
	auto pair = state.find(slot);
	if(pair == state.end()) return false;

	return pair->second;
}

void LEDController::clear(){
	for(const auto& pair : Pins){
		set(pair.first, false);
	}
}
