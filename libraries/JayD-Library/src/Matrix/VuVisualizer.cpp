#include "VuVisualizer.h"
#include <Arduino.h>

VuVisualizer::VuVisualizer(MatrixPartition *partition) : Visualizer(partition) {

}

InfoGenerator *VuVisualizer::getInfoGenerator() {
	return &info;
}

void VuVisualizer::loop(uint _millis) {
	matrix->vu(min(int(info.getVu()*2.5), INT16_MAX));
	matrix->push();
}

