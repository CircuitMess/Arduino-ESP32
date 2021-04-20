#ifndef JAYD_EFFECTPROCESSOR_H
#define JAYD_EFFECTPROCESSOR_H

#include <Arduino.h>
#include "Generator.h"
#include "Effect.h"
#include <vector>

class EffectProcessor : public Generator
{
public:
	EffectProcessor(Generator* generator);
	~EffectProcessor();
	size_t generate(int16_t* outBuffer) override;
	int available() override;

	void addEffect(Effect* effect);
	void removeEffect(int index);
	Effect* getEffect(int index);
	void setEffect(int index, Effect* effect);

	void setSource(Generator* inputGenerator);

private:
	std::vector<Effect*> effectList;
	int16_t *effectBuffer = nullptr;
	Generator* inputGenerator;
};
#endif