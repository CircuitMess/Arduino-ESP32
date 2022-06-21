#ifndef JAYD_SOURCE_H
#define JAYD_SOURCE_H

#include "Generator.h"
#include <FS.h>
#include "../Data/DataSource.h"

class Source : public Generator
{
public:
	Source(DataSource &ds);
	virtual int getBytesPerSample();
	virtual int getSampleRate();
	virtual int getChannels();

	virtual uint16_t getDuration() = 0;
	virtual uint16_t getElapsed() = 0;
	virtual void seek(uint16_t time, fs::SeekMode mode = fs::SeekSet) = 0;

	virtual void setVolume(uint8_t volume);
	uint8_t getVolume() const;
	void setRepeat(bool repeat);

	virtual void close() = 0;

protected:
	uint8_t channels = 0;
	uint32_t sampleRate = 0;
	uint8_t bytesPerSample = 0;

	float volume = 1.0f;
	bool repeat = false;

	DataSource &ds;
};

#endif