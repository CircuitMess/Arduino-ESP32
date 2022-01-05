#ifndef JAYD_EFFECT_H
#define JAYD_EFFECT_H

class Effect
{
public:
	virtual ~Effect() = default;
	virtual void applyEffect(int16_t *inBuffer, int16_t *outBuffer, size_t numSamples) = 0;
	virtual void setIntensity(uint8_t intensity) = 0;
};

#endif