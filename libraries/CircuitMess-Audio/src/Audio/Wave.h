#ifndef CIRCUITMESS_AUDIO_WAVE_H
#define CIRCUITMESS_AUDIO_WAVE_H

#include "Generator.h"

class Wave : public Generator {
public:
	enum Type {
		SINE, SAW, SQUARE, COUNT
	};

	Wave(uint16_t freq, uint16_t duration, Type type);

	void end();
	bool isDone() const;

	size_t generate(int16_t* outBuffer) override;
	int available() override;

private:
	uint16_t freq;
	uint16_t duration;
	Type type;

	size_t sampleCount;
	size_t sampleIndex = 0;
	size_t count;
	size_t current = 0;

	bool done = false;

	static double fast_sin(double x);

	static double (*generator[Type::COUNT])(uint16_t freq, size_t index);
};


#endif //CIRCUITMESS_AUDIO_WAVE_H
