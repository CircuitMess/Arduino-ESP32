#include "MatrixMid.h"


MatrixMid::MatrixMid(LEDmatrixImpl* matrix) : MatrixPartition(matrix, 12, 2){ }

void MatrixMid::push(){
		uint8_t map[4] = { 136, 120, 104, 108};


	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			if(j > 7){
				matrix->drawPixel(map[i + 2] + j - 8, buffer[i * width + j]);
			}else{
				matrix->drawPixel(map[i] + j, buffer[i * width + j]);
			}
		}
	}
}

void MatrixMid::vu(uint16_t amp){
	clear();
	uint16_t total = ((float) amp / (float)255.0) * (float) (width*255);
	uint16_t totalUpper = (float) (width*255) - total;
	for(int i = 0; i < width; i++){
		if(totalUpper < i*255) {
			break;
		}else if(totalUpper > (i+1)*255) {
			drawPixel(i, 0, 255);
		}else{
			drawPixel(i, 0,totalUpper - i*255);
		}
	}
	for(int i = 0; i < width; i++){
		if(total < i*255) {
			break;
		}else if(total > (i+1)*255) {
			drawPixel(width - 1 - i, 1, 255);
		}else{
			drawPixel(width - 1 - i, 1,total - i*255);
		}
	}
}