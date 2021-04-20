#include "MatrixBig.h"


MatrixBig::MatrixBig(LEDmatrixImpl* matrix) : MatrixPartition(matrix, 8, 9){ }

void MatrixBig::push(){
	uint8_t map[9] = { 0, 16, 32, 48, 64, 80, 96, 112, 128 };

	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			matrix->drawPixel(map[i] + j, buffer[(i+1) * width - j - 1]);
		}
	}
}