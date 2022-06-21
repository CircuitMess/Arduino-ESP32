#include "MatrixR.h"

MatrixR::MatrixR(MatrixOutputBuffer* output) : MatrixPartOutput(output, 3, 8){}

std::pair<uint16_t, uint16_t> MatrixR::map(uint16_t x, uint16_t y){
	return { y + 8, x + 3};
}