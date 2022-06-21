#include "VuVisualizer.h"
#include <Arduino.h>

VuVisualizer::VuVisualizer(Matrix *matrix) : Visualizer(matrix) {

}

InfoGenerator *VuVisualizer::getInfoGenerator() {
	return &info;
}

void VuVisualizer::loop(uint _millis) {
	vu(min(int(info.getVu()*2.5), INT16_MAX));
	matrix->push();
}

void VuVisualizer::vu(uint16_t amp){
	matrix->clear();
	uint16_t total = ((float) amp / (float)INT16_MAX) * (float) (matrix->getHeight()*255);
	for(int i = 0; i < matrix->getHeight(); i++){
		for(int j = 0; j < matrix->getWidth(); j++){
			if(total < i*255) {
				break;
			}else if(total > (i+1)*255) {
				matrix->drawPixel(j, matrix->getHeight() - 1 - i, {255, 255, 255, 255});
			}else{
				matrix->drawPixel(j, matrix->getHeight() - 1 - i, {255, 255, 255, (uint8_t)(total - i*255)});
			}
		}
	}
	matrix->push();
}

