#include "MatrixL.h"

MatrixL::MatrixL(LEDmatrixImpl* matrix) : MatrixPartition(matrix, 3, 8){ }

void MatrixL::push(){
	uint8_t map[3] = { 40, 24, 8 };

	for(int i = 0; i < width; i++){
		for(int j = 0; j < height; j++){
			matrix->drawPixel(map[i] + j, buffer[(height - j - 1) * width + i]);
		}
	}
}
