#ifndef JAYD_SOURCEWAV_H
#define JAYD_SOURCEWAV_H

#include <Arduino.h>
#include "../Setup.hpp"
#include <FS.h>
#include "Source.h"
#include "../Data/Scheduler.h"
#include <Buffer/RingBuffer.h>

class SourceWAV : public Source
{
public:
	SourceWAV(DataSource& ds);
	size_t generate(int16_t* outBuffer) override;
	int available() override;

	uint16_t getDuration() override;
	uint16_t getElapsed() override;
	void seek(uint16_t time, fs::SeekMode mode) override;

	void close() override;

private:

	size_t dataSize;
	size_t readData = 0;
	bool readHeader();
};


#endif