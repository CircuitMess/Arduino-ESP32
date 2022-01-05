#ifndef JAYD_SOURCEAAC_H
#define JAYD_SOURCEAAC_H

#include <Arduino.h>
#include "../Setup.hpp"
#include <FS.h>
#include "Source.h"
#include "../SDScheduler.h"
#include "Decoder/libhelix-aac/aacdec.h"
#include <Buffer/RingBuffer.h>
#include <aacenc_lib.h>
#include <aacdecoder_lib.h>
#include <Buffer/DataBuffer.h>

class SourceAAC : public Source
{
public:
	SourceAAC();
	SourceAAC(fs::File file);
	~SourceAAC();
	size_t generate(int16_t* outBuffer) override;
	int available() override;

	uint16_t getDuration() override;
	uint16_t getElapsed() override;
	void seek(uint16_t time, fs::SeekMode mode) override;

	void open(fs::File file);

	void close() override;

	void setVolume(uint8_t volume);

	void setRepeat(bool repeat);
	void setSongDoneCallback(void (*callback)());

private:
	fs::File file;
	uint32_t bitrate = 0;

	size_t dataSize = 0;
	size_t movedBytes = 0;

	float volume = 1.0f;

	RingBuffer readBuffer;
	bool readJobPending = false;

	DataBuffer fillBuffer;
	DataBuffer dataBuffer;
	void refill();

	HAACDecoder hAACDecoder = nullptr;

	struct ADTSHeader {
		unsigned char syncword_0_to_8: 8;

		unsigned char protection_absent: 1;
		unsigned char layer: 2;
		unsigned char ID: 1;
		unsigned char syncword_9_to_12: 4;

		unsigned char channel_configuration_0_bit: 1;
		unsigned char private_bit: 1;
		unsigned char sampling_frequency_index: 4;
		unsigned char profile: 2;

		unsigned char frame_length_0_to_1: 2;
		unsigned char copyrignt_identification_start: 1;
		unsigned char copyright_identification_bit: 1;
		unsigned char home: 1;
		unsigned char original_or_copy: 1;
		unsigned char channel_configuration_1_to_2: 2;

		unsigned char frame_length_2_to_9: 8;

		unsigned char adts_buffer_fullness_0_to_4: 5;
		unsigned char frame_length_10_to_12: 3;

		unsigned char number_of_raw_data_blocks_in_frame: 2;
		unsigned char adts_buffer_fullness_5_to_10: 6;
	};


	SDResult* readResult = nullptr;
	void addReadJob(bool full = false);
	void processReadJob();
	void resetDecoding();

	bool repeat = false;
	void (*songDoneCallback)() = nullptr;
};


#endif