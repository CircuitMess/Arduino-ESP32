#include "MatrixMid.h"


MatrixMid::MatrixMid(MatrixOutputBuffer* output) : MatrixPartOutput(output, 12, 2){}

std::pair<uint16_t, uint16_t> MatrixMid::map(uint16_t x, uint16_t y){
	static const uint8_t map[4] = { 136, 120, 104, 108 };

	if(x > 7){
		return { x + y*4, 6 };
	}else{
		return { 8 + x, 8 - y };
	}
}
