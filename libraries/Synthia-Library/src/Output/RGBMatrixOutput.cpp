#include <Util/Timer.h>
#include "RGBMatrixOutput.h"

RGBMatrixOutput::RGBMatrixOutput() : MatrixOutput(10, 1){

}

void RGBMatrixOutput::set(ShiftOutput* output, const std::array<PixelMapping, 10>& map){
	this->output = output;
	this->map = map;
}

void RGBMatrixOutput::init(){
	output->setAll(true);
}

void RGBMatrixOutput::push(const MatrixPixelData& data){
	auto state = output->get();

	for(int i = 0; i < 10; i++){
		auto pinR = map[i].pinR;
		auto pinG = map[i].pinG;
		auto pinB = map[i].pinB;
		auto pixel = data.get(i, 0);

		state[pinR.index][pinR.pin] = (pixel.r * pixel.i / 255) < 255 / 2;
		state[pinG.index][pinG.pin] = (pixel.g * pixel.i / 255) < 255 / 2;
		state[pinB.index][pinB.pin] = (pixel.b * pixel.i / 255) < 255 / 2;
	}

	output->set(state);
}
