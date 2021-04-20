#ifndef JAYD_LIBRARY_PLAYBACKSYSTEM_H
#define JAYD_LIBRARY_PLAYBACKSYSTEM_H


#include <Util/Task.h>
#include "../OutputI2S.h"
#include "../SpeedModifier.h"
#include "../EffectProcessor.h"
#include "../Mixer.h"
#include "../SourceWAV.h"
#include "../EffectType.hpp"
#include "../SourceAAC.h"
#include <Sync/Queue.h>

struct PlaybackRequest {
	enum { SEEK } type;
	size_t value;
};

class PlaybackSystem {
public:
	PlaybackSystem();
	PlaybackSystem(const fs::File& f);
	virtual ~PlaybackSystem();

	bool open(const fs::File& file);

	Task audioTask;
	static void audioThread(Task* task);

	void start();
	void stop();
	bool isRunning();

	uint16_t getDuration();
	uint16_t getElapsed();

	void setVolume(uint8_t volume);
	void seek(uint16_t time);

	void setRepeat(bool repeat = true);
	void updateGain();

private:
	bool running = false;
	bool repeat = false;
	Queue queue;

	fs::File file;

	SourceAAC* source = nullptr;
	OutputI2S* out;

	void _seek(uint16_t time);
};

#endif //JAYD_LIBRARY_PLAYBACKSYSTEM_H
