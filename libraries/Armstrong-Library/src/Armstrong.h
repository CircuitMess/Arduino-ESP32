#ifndef ARMSTRONG_LIBRARY_ARMSTRONG_H
#define ARMSTRONG_LIBRARY_ARMSTRONG_H

#include <CircuitOS.h>
#include <Input/InputShift.h>
#include "Names.h"
#include "EncoderInput.h"
#include "ServoControl.h"
#include "LEDController.h"
#include "RGBController.h"
#include "ColorSensor.h"

class ArmstrongImpl {
public:
	ArmstrongImpl();
	void begin();

	InputShift* getInput();

	Slot btnToSlot(uint8_t i);

private:
	InputShift* input;

	const std::unordered_map<uint8_t, Slot> BtnSLotMap = {
			{ BTN_1, Slot::Pos1 },
			{ BTN_2, Slot::Pos2 },
			{ BTN_3, Slot::Pos3 },
			{ BTN_4, Slot::Pos4 },
			{ BTN_PP, Slot::PlayPause }
	};

};

extern ArmstrongImpl Armstrong;
extern EncoderInput Encoders;
extern ServoControl Motors;
extern LEDController LED;
extern RGBController RGB;
extern ColorSensor Sensor;

#endif //ARMSTRONG_LIBRARY_ARMSTRONG_H
