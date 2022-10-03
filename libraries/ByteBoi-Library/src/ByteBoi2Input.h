#ifndef CIRCUITOS_INPUTSHIFT_H
#define CIRCUITOS_INPUTSHIFT_H

#include <Input/Input.h>

class ByteBoi2Input : public Input {
public:
	ByteBoi2Input(uint8_t dataPin, uint8_t clockPin, uint8_t loadPin, uint8_t numButtons);
	void begin();

protected:
	void scanButtons() override;

private:
	uint8_t dataPin;
	uint8_t clockPin;
	uint8_t loadPin;

	uint8_t numButtons;
	uint8_t numShifts;

	// actual -> nice
	const std::map<int, int> remap = {
			{ 7, 0 },
			{ 6, 2 },
			{ 5, 1 },
			{ 4, 3 },
			{ 3, 6 },
			{ 2, 5 },
			{ 1, 4 },
			{ 0, 7 },
	};

};

#endif //CIRCUITOS_INPUTSHIFT_H
