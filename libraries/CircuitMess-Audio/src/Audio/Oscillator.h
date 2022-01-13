#ifndef CIRCUITMESS_AUDIO_OSCILLATOR_H
#define CIRCUITMESS_AUDIO_OSCILLATOR_H

#include <Arduino.h>
#include <unordered_set>
#include <Sync/Mutex.h>
#include "Generator.h"
#include "Wave.h"

class Oscillator : public Generator {
public:
	Oscillator();
	virtual ~Oscillator();

	int available() override;
	size_t generate(int16_t* outBuffer) override;

	void tone(uint16_t freq, uint16_t duration, Wave::Type type = Wave::SINE);
	void noTone();

private:
	std::unordered_set<Wave*> waves;
	std::unordered_set<Wave*> pending;

	Mutex pendingMutex;

	int16_t* waveBuffer = nullptr;
};


#endif //CIRCUITMESS_AUDIO_OSCILLATOR_H
