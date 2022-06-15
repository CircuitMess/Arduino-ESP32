#include "TrackMatrixOutput.h"

TrackMatrixOutput::TrackMatrixOutput(MatrixOutputBuffer* output) : MatrixPartOutput(output, 16, 5){}

std::pair<uint16_t, uint16_t> TrackMatrixOutput::map(uint16_t x, uint16_t y){
	return {x, y};
}

