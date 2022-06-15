#include "MatrixBig.h"


MatrixBig::MatrixBig(MatrixOutputBuffer* output) : MatrixPartOutput(output, 8, 9){}

std::pair<uint16_t, uint16_t> MatrixBig::map(uint16_t x, uint16_t y){
	uint8_t map[9] = { 0, 16, 32, 48, 64, 80, 96, 112, 128 };
	return {getWidth() - x - 1, y};
}