#include "TrackRGBOutput.h"

TrackRGBOutput::TrackRGBOutput(MatrixOutputBuffer* output) : MatrixPartOutput(output, 1, 5){}

std::pair<uint16_t, uint16_t> TrackRGBOutput::map(uint16_t x, uint16_t y){
	return { y, 0 };
}
