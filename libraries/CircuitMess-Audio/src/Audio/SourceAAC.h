#ifndef JAYD_SOURCEAAC_H
#define JAYD_SOURCEAAC_H

#include <Arduino.h>
#include "../Setup.hpp"
#include <FS.h>
#include "Source.h"
#include "../Data/Scheduler.h"
#include "Decoder/libhelix-aac/aacdec.h"
#include <Buffer/RingBuffer.h>
#include <aacenc_lib.h>
#include <aacdecoder_lib.h>
#include <Buffer/DataBuffer.h>

class SourceAAC : public Source
{
public:
	SourceAAC(DataSource& ds);

	~SourceAAC();

	size_t generate(int16_t* outBuffer) override;
	int available() override;
	void close() override;

	uint16_t getDuration() override;
	uint16_t getElapsed() override;
	void seek(uint16_t time, fs::SeekMode mode) override;

	void setSongDoneCallback(void (*callback)());

private:
	uint32_t bitrate = 0;

	size_t dataSize = 0;
	size_t movedBytes = 0;


	DataBuffer fillBuffer;
	DataBuffer dataBuffer;
	void refill();

	HAACDecoder hAACDecoder = nullptr;

	void resetDecoding();

	void (*songDoneCallback)() = nullptr;
};


#endif