#include "CursorMatrixOutput.h"

CursorMatrixOutput::CursorMatrixOutput(MatrixOutputBuffer* output) : MatrixPartOutput(output, 16, 1){}

std::pair<uint16_t, uint16_t> CursorMatrixOutput::map(uint16_t x, uint16_t y){
	return {x, y + 5};
}
