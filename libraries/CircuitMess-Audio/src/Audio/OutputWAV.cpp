#include "OutputWAV.h"
#include <string>
#include "../Setup.hpp"
#include "../PerfMon.h"


struct WavHeader{
	char RIFF[4];
	uint32_t chunkSize;
	char WAVE[4];
	char fmt[3];
	uint32_t fmtSize;
	uint16_t audioFormat;
	uint16_t numChannels;
	uint32_t sampleRate;
	uint32_t byteRate; // == SampleRate * NumChannels * BitsPerSample/8
	uint16_t blockAlign; // == NumChannels * BitsPerSample/8
	uint16_t bitsPerSample;
	char data[4];
	uint32_t dataSize; // == NumSamples * NumChannels * BitsPerSample/8
};

OutputWAV::OutputWAV(){
	freeBuffers.reserve(OUTWAV_BUFCOUNT);
	for(int i = 0; i < OUTWAV_BUFCOUNT; i++){
		outBuffers[i] = new DataBuffer(OUTWAV_BUFSIZE);
		freeBuffers.push_back(i);
	}
}

OutputWAV::OutputWAV(const fs::File& file) : OutputWAV(){
	this->file = file;
}

OutputWAV::~OutputWAV(){
	for(auto& outBuffer : outBuffers){
		delete outBuffer;
	}
}

const fs::File& OutputWAV::getFile() const{
	return file;
}

void OutputWAV::setFile(const fs::File& file){
	OutputWAV::file = file;
}

void OutputWAV::output(size_t numSamples){
	Profiler.start("WAV write process");
	processWriteJob();
	Profiler.end();

	while(freeBuffers.empty()){
		processWriteJob();
	}

	DataBuffer* buffer = outBuffers[freeBuffers.front()];

	size_t size = numSamples * NUM_CHANNELS * BYTES_PER_SAMPLE;
	dataLength += size;

	memcpy(buffer->writeData(), this->inBuffer, size);
	buffer->writeMove(size);

	if(buffer->readAvailable() >= OUTWAV_WRITESIZE){
		Profiler.start("WAV write add");
		addWriteJob();
		Profiler.end();
	}
}

void OutputWAV::init(){
	dataLength = 0;

	if(!file){
		Serial.println("Output file not open");
		return;
	}

	writeHeaderWAV(0);
}

void OutputWAV::deinit(){
	Serial.println("Stopping WAV");
	if(!freeBuffers.empty() && outBuffers[freeBuffers.front()]->readAvailable() > 0){
		Serial.println("Writing last buffer");
		addWriteJob();
	}

	Serial.println("Waiting jobs");

	while(freeBuffers.size() != OUTWAV_BUFCOUNT){
		processWriteJob();
	}

	Serial.println("Writing header");

	writeHeaderWAV(dataLength);
}

void OutputWAV::addWriteJob(){
	if(freeBuffers.empty()) return;
	uint8_t i = freeBuffers.front();

	Sched.addJob(new SDJob{
			 .type = SDJob::SD_WRITE,
			 .file = file,
			 .size = outBuffers[i]->readAvailable(),
			 .buffer = const_cast<uint8_t*>(outBuffers[i]->readData()),
			 .result = &writeResult[i]
	 });

	freeBuffers.erase(freeBuffers.begin());
	writePending[i] = true;
}

void OutputWAV::processWriteJob(){
	for(int i = 0; i < OUTWAV_BUFCOUNT; i++){
		if(!writePending[i]) continue;
		if(writeResult[i] == nullptr) continue;

		outBuffers[i]->clear();

		delete writeResult[i];
		writeResult[i] = nullptr;

		writePending[i] = false;
		freeBuffers.push_back(i);
	}
}

void OutputWAV::writeHeaderWAV(size_t size){
	WavHeader header;
	memcpy(header.RIFF, "RIFF", 4);
	header.chunkSize = size + 36;
	memcpy(header.WAVE, "WAVE", 4);
	memcpy(header.fmt, "fmt ", 4);
	header.fmtSize = 16;
	header.audioFormat = 1; //PCM
	header.numChannels = NUM_CHANNELS; //2 channels
	header.sampleRate = SAMPLE_RATE;
	header.byteRate = SAMPLE_RATE * NUM_CHANNELS * BYTES_PER_SAMPLE;
	header.blockAlign = NUM_CHANNELS * BYTES_PER_SAMPLE;
	header.bitsPerSample = BYTES_PER_SAMPLE * 8;
	memcpy(header.data, "data", 4);
	header.dataSize = size;

	Sched.addJob(new SDJob {
		.type = SDJob::SD_SEEK,
		.file = file,
		.size = 0,
		.buffer = nullptr,
		.result = nullptr
	});

	Sched.addJob(new SDJob {
			.type = SDJob::SD_SEEK,
			.file = file,
			.size = 0,
			.buffer = nullptr,
			.result = nullptr
	});

	SDResult* result = nullptr;
	Sched.addJob(new SDJob {
			.type = SDJob::SD_WRITE,
			.file = file,
			.size = sizeof(WavHeader),
			.buffer = reinterpret_cast<uint8_t*>(&header),
			.result = &result
	});

	while(result == nullptr){
		delayMicroseconds(1);
	}

	delete result;
}
