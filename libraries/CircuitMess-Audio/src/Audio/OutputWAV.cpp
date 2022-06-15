#include "OutputWAV.h"
#include <string>
#include "../Setup.hpp"
#include "../PerfMon.h"

OutputWAV::OutputWAV(){
/*	freeBuffers.reserve(OUTWAV_BUFCOUNT);
	for(int i = 0; i < OUTWAV_BUFCOUNT; i++){
		outBuffers[i] = new DataBuffer(OUTWAV_BUFSIZE);
		freeBuffers.push_back(i);
	}*/
}

OutputWAV::OutputWAV(const fs::File& file) : OutputWAV(){
	this->file = file;
}

OutputWAV::~OutputWAV(){
/*	for(auto& outBuffer : outBuffers){
		delete outBuffer;
	}*/
}

const fs::File& OutputWAV::getFile() const{
	return file;
}

void OutputWAV::setFile(const fs::File& file){
	OutputWAV::file = file;
}

void OutputWAV::output(size_t numSamples){
/*	Profiler.start("WAV write process");
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
	}*/

/*
	for(int i = 0; i < numSamples; i++){
		printf("%d\n", inBuffer[i]);
	}
*/

	size_t size = numSamples * NUM_CHANNELS * BYTES_PER_SAMPLE;
	file.write((uint8_t*)inBuffer, size);
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
/*	Serial.println("Stopping WAV");
	if(!freeBuffers.empty() && outBuffers[freeBuffers.front()]->readAvailable() > 0){
		Serial.println("Writing last buffer");
		addWriteJob();
	}

	Serial.println("Waiting jobs");

	while(freeBuffers.size() != OUTWAV_BUFCOUNT){
		processWriteJob();
	}*/

	Serial.println("Writing header");

	writeHeaderWAV(dataLength);
}

/*void OutputWAV::addWriteJob(){
	if(freeBuffers.empty()) return;
	uint8_t i = freeBuffers.front();

	Sched.addJob(new SchedJob{
			 .type = SchedJob::WRITE,
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
}*/

void OutputWAV::writeHeaderWAV(size_t size){
	struct HeaderWAV {
		char RIFF[4] = { 'R', 'I', 'F', 'F' };
		uint32_t chunkSize = 36;
		char WAVE[4] = { 'W', 'A', 'V', 'E' };
		char fmt[4] = { 'f', 'm', 't', ' ' };
		uint32_t fmtSize = 16;
		uint16_t audioFormat = 1; // PCM
		uint16_t numChannels = NUM_CHANNELS;
		uint32_t sampleRate = SAMPLE_RATE;
		uint32_t byteRate = SAMPLE_RATE * NUM_CHANNELS * BYTES_PER_SAMPLE;
		uint16_t blockAlign = NUM_CHANNELS * BYTES_PER_SAMPLE;
		uint16_t bitsPerSample = BYTES_PER_SAMPLE * 8;
		char data[4] = { 'd', 'a', 't', 'a' };
		uint32_t dataSize = 0;
	};

	HeaderWAV header;
	header.chunkSize += size;
	header.dataSize = size;

	file.seek(0);
	file.write(reinterpret_cast<const uint8_t*>(&header), sizeof(HeaderWAV));
}
