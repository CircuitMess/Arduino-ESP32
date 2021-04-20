#include "MatrixR.h"

MatrixR::MatrixR(LEDmatrixImpl* matrix) : MatrixPartition(matrix, 3, 8){ }

void MatrixR::push(){
	uint8_t map[3] = { 56, 72, 88 };

	for(int i = 0; i < width; i++){
		for(int j = 0; j < height; j++){
			matrix->drawPixel(map[i] + j, buffer[j * width + i]);
		}
	}
}