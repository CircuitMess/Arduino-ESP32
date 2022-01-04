#ifndef BYTEBOI_LIBRARY_PLAYBACKSYSTEM_H
#define BYTEBOI_LIBRARY_PLAYBACKSYSTEM_H


#include <CMAudio.h>
#include <Audio/OutputI2S.h>
#include <Audio/SpeedModifier.h>
#include <Audio/EffectProcessor.h>
#include <Audio/Mixer.h>
#include <Audio/SourceWAV.h>
#include <Audio/EffectType.hpp>
#include <Audio/SourceAAC.h>
#include <Audio/OutputDAC.h>
#include <Util/Task.h>
#include <Sync/Queue.h>
#include <Audio/Oscillator.h>
#include <Audio/Mixer.h>
#include <Audio/Wave.h>

class Sample;

struct PlaybackRequest {
	enum { SEEK } type;
	size_t value;
};

class PlaybackSystem {
public:
	PlaybackSystem();
	PlaybackSystem(Sample* sample);
	virtual ~PlaybackSystem();

	void begin();

	void tone(uint16_t freq, uint16_t duration, Wave::Type type = Wave::SINE);
	void noTone();

	bool open(Sample* sample);
	void play(Sample* sample);
	void start();
	void stop();
	bool isRunning();

	Task audioTask;
	static void audioThread(Task* task);

	uint16_t getDuration();
	uint16_t getElapsed();

	void seek(uint16_t time);

	void disableScheduler(bool schedDisabled);
	void updateGain();

private:
	bool running = false;
	Queue queue;
	bool schedDisabled = false;

	OutputDAC* out = nullptr;
	Oscillator* oscillator = nullptr;
	Mixer* mixer = nullptr;

	Sample* currentSample = nullptr;

	void _seek(uint16_t time);

	void setMixRatio();
};

extern PlaybackSystem Playback;

#endif //BYTEBOI_LIBRARY_PLAYBACKSYSTEM_H
