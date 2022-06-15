#ifndef JAYD_SOURCEMP3_H
#define JAYD_SOURCEMP3_H

#include <Arduino.h>
#include "../Setup.hpp"
#include <FS.h>
#include "Source.h"
#include "../Data/Scheduler.h"
#include "Decoder/libhelix-mp3/mp3dec.h"
#include "Decoder/ID3Parser.h"
#include <Buffer/DataBuffer.h>

class SourceMP3 : public Source
{
public:
	SourceMP3(DataSource& ds);
	~SourceMP3();
	size_t generate(int16_t* outBuffer) override;
	int available() override;

	uint16_t getDuration() override;
	uint16_t getElapsed() override;
	void seek(uint16_t time, fs::SeekMode mode) override;

	void refill();
	void close() override;

	const ID3Metadata& getMetadata() const;

private:
	uint32_t bitrate = 0;

	size_t readData = 0;

	DataBuffer fillBuffer;
	DataBuffer dataBuffer;

	HMP3Decoder decoder = nullptr;

	ID3Metadata metadata;
};


#endif