#include "VuInfoGenerator.h"
#include "../Setup.hpp"

void VuInfoGenerator::captureInfo(int16_t *outBuffer, size_t readSamples) {
	float EMA_a = 0.01;
	vu = abs(outBuffer[0]);
	for (int i = 1; i < readSamples; ++i) {
		vu = (EMA_a * abs(outBuffer[i])) + ((1 - EMA_a) * vu);
	}
}

uint16_t VuInfoGenerator::getVu() {
	return vu;
}
