#include "RoundVuVisualiser.h"

RoundVuVisualiser::RoundVuVisualiser(Matrix *matrix) : Visualizer(matrix) {

}

InfoGenerator *RoundVuVisualiser::getInfoGenerator() {
	return &info;
}

void RoundVuVisualiser::loop(uint _millis) {
	matrix->clear();
	uint16_t total = ((float) min(int(info.getVu()*2.5), INT16_MAX) / (float)INT16_MAX) * (float) (4*255);
	for(int i = 0; i < 4; i++){
		if(total < i*255) {
			break;
		}else if(total > (i+1)*255) {
			drawSquare(i+1, 255);
		}else{
			drawSquare(i+1, total - i*255);
		}
	}
	matrix->push();
}

void RoundVuVisualiser::drawSquare(uint8_t width, uint8_t value) {
	int i, j;
	for (i = 0; i < 2*width; i++)
	{
		for (j = 0; j < (2*width-1); j++)
		{
			if (i == 0 || i == 2*width-1 ||
				j == 0 || j == (2*width-2))
				matrix->drawPixel(4-width + i, 4 - width + 1 +j, {255, 255, 255, value});
		}
	}
}
