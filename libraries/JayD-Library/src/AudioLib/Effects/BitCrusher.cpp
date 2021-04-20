#include "BitCrusher.h"

BitCrusher::BitCrusher(){


}

void BitCrusher::applyEffect(int16_t *inBuffer, int16_t *outBuffer, size_t numSamples){
	for(int i = 0; i < numSamples; ++i){
		outBuffer[i] = signalProcessing(inBuffer[i]);
	}
}

int16_t BitCrusher::signalProcessing(int16_t sample){
	sample /= scaleFactor;
	sample *= scaleFactor;

	return sample;
}

void BitCrusher::setIntensity(uint8_t intensity){
	scaleFactor = ((float) intensity / 255.0f) * 4096.0f;
	scaleFactor = max((uint16_t) 1, scaleFactor);
}