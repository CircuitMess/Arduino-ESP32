#ifndef JAYD_SOURCEMP3_H
#define JAYD_SOURCEMP3_H

#include <Arduino.h>
#include "../Setup.hpp"
#include <FS.h>
#include "Source.h"
#include "../SDScheduler.h"
#include "Decoder/libhelix-mp3/mp3dec.h"
#include "Decoder/ID3Parser.h"
#include "SourceMP3.h"
#include <Buffer/RingBuffer.h>
#include <Buffer/DataBuffer.h>

class SourceMP3 : public Source
{
public:
	SourceMP3();
	SourceMP3(fs::File file);
	~SourceMP3();
	size_t generate(int16_t* outBuffer) override;
	int available() override;

	uint16_t getDuration() override;
	uint16_t getElapsed() override;
	void seek(uint16_t time, fs::SeekMode mode) override;

	void open(fs::File file);

	void close() override;

	void setVolume(uint8_t volume);

	const ID3Metadata& getMetadata() const;

private:
	fs::File file;
	uint32_t bitrate = 0;

	size_t dataSize = 0;
	size_t readData = 0;

	float volume = 1.0f;

	RingBuffer readBuffer;
	bool readJobPending = false;

	DataBuffer fillBuffer;
	DataBuffer dataBuffer;

	HMP3Decoder decoder = nullptr;

	ID3Metadata metadata;


	SDResult* readResult = nullptr;
	void addReadJob(bool full = false);
	void processReadJob();
};


#endif