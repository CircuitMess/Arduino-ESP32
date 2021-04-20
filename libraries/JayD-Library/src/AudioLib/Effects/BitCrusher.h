#ifndef JAYD_LIBRARY_BITCRUSHER_H
#define JAYD_LIBRARY_BITCRUSHER_H

#include <Arduino.h>
#include "../Effect.h"

class BitCrusher : public Effect {

public:

	BitCrusher();

	void applyEffect(int16_t *inBuffer, int16_t *outBuffer, size_t numSamples) override;
	void setIntensity(uint8_t intensity) override;

private:

	int16_t signalProcessing(int16_t sample);

	uint16_t scaleFactor = 1;
};


#endif //JAYD_LIBRARY_BITCRUSHER_H
