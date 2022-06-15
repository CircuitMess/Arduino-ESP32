#include "SlotRGBOutput.h"

SlotRGBOutput::SlotRGBOutput(MatrixOutputBuffer* output) : MatrixPartOutput(output, 5, 1){}

std::pair<uint16_t, uint16_t> SlotRGBOutput::map(uint16_t x, uint16_t y){
	return { x + 5, 0 };
}
