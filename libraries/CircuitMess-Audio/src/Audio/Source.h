#ifndef JAYD_SOURCE_H
#define JAYD_SOURCE_H

#include "Generator.h"
#include <FS.h>

class Source : public Generator
{
public:
	virtual int getBytesPerSample();
	virtual int getSampleRate();
	virtual int getChannels();

	virtual uint16_t getDuration() = 0;
	virtual uint16_t getElapsed() = 0;
	virtual void seek(uint16_t time, fs::SeekMode mode) = 0;

	virtual void close() = 0;

protected:
	uint8_t channels = 0;
	uint32_t sampleRate = 0;
	uint8_t bytesPerSample = 0;
};

#endif