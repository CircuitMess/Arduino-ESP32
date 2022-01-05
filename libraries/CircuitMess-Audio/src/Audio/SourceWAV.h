#ifndef JAYD_SOURCEWAV_H
#define JAYD_SOURCEWAV_H

#include <Arduino.h>
#include "../Setup.hpp"
#include <FS.h>
#include "Source.h"
#include "../SDScheduler.h"
#include <Buffer/RingBuffer.h>

class SourceWAV : public Source
{
public:
	SourceWAV();
	SourceWAV(fs::File file);
	~SourceWAV();
	size_t generate(int16_t* outBuffer) override;
	int available() override;

	uint16_t getDuration() override;
	uint16_t getElapsed() override;
	void seek(uint16_t time, fs::SeekMode mode) override;

	void open(fs::File file);

	void close() override;

	void setVolume(uint8_t volume);

private:
	fs::File file;

	size_t dataSize;
	size_t readData = 0;
	bool readHeader();

	float volume = 1.0f;

	RingBuffer readBuffer;
	bool readJobPending = false;
	static const size_t readChunkSize = BUFFER_SIZE * 4;

	SDResult* readResult = nullptr;
	void addReadJob(bool full = false);
	void processReadJob();
};


#endif