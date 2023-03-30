#ifndef BATMOBILE_LIBRARY_SIMPLEPLAYBACKSYSTEM_H
#define BATMOBILE_LIBRARY_SIMPLEPLAYBACKSYSTEM_H

#include <Util/Task.h>
#include <Sync/Queue.h>
#include <Audio/OutputDAC.h>
#include <Audio/SourceAAC.h>
#include <Data/FileDataSource.h>

class AudioSystem {
public:
	AudioSystem();
	virtual ~AudioSystem();

	void begin();

	void play(File file);
    void playRepeating(File file);
	void stop();

	void setVolume(uint8_t volume);

	bool isPlaying() const;

private:
	OutputDAC output;

	FileDataSource* dataSource = nullptr;
	SourceAAC* source = nullptr;

	Task task;

	struct PlayJob {
		FileDataSource* dataSource;
        bool repeating = false;
	};
	Queue playQueue;

	void player();

	bool running = false;

};

#endif //BATMOBILE_LIBRARY_SIMPLEPLAYBACKSYSTEM_H
