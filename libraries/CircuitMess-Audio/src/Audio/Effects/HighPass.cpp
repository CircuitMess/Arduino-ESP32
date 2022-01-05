#include "HighPass.h"

HighPass::HighPass(){
	HighPass::setIntensity(0);
}

void HighPass::applyEffect(int16_t *inBuffer, int16_t *outBuffer, size_t numSamples){
	for(int i = 0; i < numSamples; ++i){
		outBuffer[i] = signalProcessing(inBuffer[i]);
	}
}

int16_t HighPass::signalProcessing(int16_t sample){
	filter = filter2;
	filter2 = sample;

	float filtered = ((float) sample * fAmpI) + ((filter2 - filter) * fAmp);
	filtered = constrain(filtered, -32768, 32767);

	return filtered;
}

void HighPass::setIntensity(uint8_t intensity){
	float val = (float)intensity/255.0f;
	fAmp = val;
	fAmpI = 1 - val;
}
