#ifndef JAYD_LIBRARY_LOWPASS_H
#define JAYD_LIBRARY_LOWPASS_H

#include <Arduino.h>
#include "../Effect.h"

class LowPass : public Effect{

public:

	LowPass();

	void applyEffect(int16_t *inBuffer, int16_t *outBuffer, size_t numSamples) override;

	void setIntensity(uint8_t intensity) override;

private:

	int16_t signalProcessing(int16_t sample);

	float filter;
	float filter2;

	float fAmp;
	float fAmpI;
};

#endif //JAYD_LIBRARY_LOWPASS_H
