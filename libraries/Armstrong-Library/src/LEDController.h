#ifndef ARMSTRONG_LIBRARY_LEDCONTROLLER_H
#define ARMSTRONG_LIBRARY_LEDCONTROLLER_H

#include <Arduino.h>
#include <Devices/ShiftOutput.h>
#include "Pins.hpp"
#include "Names.h"
#include <unordered_map>

class LEDController {
public:
	LEDController();

	void begin();
	void end();

	void set(Slot slot, bool value);
	bool get(Slot slot) const;
	void clear();

private:
	const std::unordered_map<Slot, uint8_t, SlotHash> Pins = {
			{ Slot::Pos1, LED_1 },
			{ Slot::Pos2, LED_2 },
			{ Slot::Pos3, LED_3 },
			{ Slot::Pos4, LED_4 },
			{ Slot::PlayPause, LED_PP }
	};

	ShiftOutput output;

	std::unordered_map<Slot, bool, SlotHash> state;

};


#endif //ARMSTRONG_LIBRARY_LEDCONTROLLER_H
