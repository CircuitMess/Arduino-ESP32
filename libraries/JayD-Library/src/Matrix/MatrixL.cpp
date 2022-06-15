#include "MatrixL.h"

MatrixL::MatrixL(MatrixOutputBuffer* matrix) : MatrixPartOutput(matrix, 3, 8){}

std::pair<uint16_t, uint16_t> MatrixL::map(uint16_t x, uint16_t y){
	return { 15-y, getWidth() - x - 1 };
}
