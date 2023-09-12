#ifndef ARMSTRONG_LIBRARY_DATA_H
#define ARMSTRONG_LIBRARY_DATA_H

#include <Arduino.h>

enum Slot : uint8_t { Pos1, Pos2, Pos3, Pos4, PlayPause, None };

enum Motor : uint8_t { Rotate, Extend, Pinch, Lift };

struct SlotHash {
	std::size_t operator()(Slot s) const{
		return static_cast<std::size_t>(s);
	}
};

struct MotorHash {
	std::size_t operator()(Motor s) const{
		return static_cast<std::size_t>(s);
	}
};

#endif //ARMSTRONG_LIBRARY_DATA_H
