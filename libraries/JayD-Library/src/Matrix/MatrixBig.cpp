#include "MatrixBig.h"
#include <Util/HWRevision.h>

MatrixBig::MatrixBig(MatrixOutputBuffer* output) : MatrixPartOutput(output, 8, 9){}

std::pair<uint16_t, uint16_t> MatrixBig::map(uint16_t x, uint16_t y){
	if(HWRevision::get() == 2){
		return { x, y };
	}else{
		return { getWidth() - x - 1, y };
	}
}