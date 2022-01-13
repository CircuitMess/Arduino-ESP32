#include "LowPass.h"

LowPass::LowPass(){
	LowPass::setIntensity(0);
}

void LowPass::applyEffect(int16_t *inBuffer, int16_t *outBuffer, size_t numSamples){
	for(int i = 0; i < numSamples; ++i){
		outBuffer[i] = signalProcessing(inBuffer[i]);
	}
}

int16_t LowPass::signalProcessing(int16_t sample){
	filter = ((float)sample * fAmpI) + (filter * fAmp);
	filter2 = (filter * fAmpI) + (filter2 * fAmp);

	float filtered = filter2;
	filtered = constrain(filtered, -32768, 32767);

	return filtered;
}

void LowPass::setIntensity(uint8_t intensity){
	intensity *= 0.85f;

	float val = (float)intensity/255.0f;
	fAmp = val;
	fAmpI = 1 - val;
}
