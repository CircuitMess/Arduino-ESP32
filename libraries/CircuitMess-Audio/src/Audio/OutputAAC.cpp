#include "OutputAAC.h"
#include <string>
#include "../Setup.hpp"
#include "../PerfMon.h"

OutputAAC::OutputAAC(){
	freeBuffers.reserve(OUTFS_BUFCOUNT);
	for(int i = 0; i < OUTFS_BUFCOUNT; i++){
		outBuffers[i] = new DataBuffer(OUTFS_BUFSIZE);
		freeBuffers.push_back(i);
	}

	if(psramFound()){
		decodeBuffer = static_cast<uint8_t*>(ps_malloc(OUTFS_DECODE_BUFSIZE));
	}else{
		decodeBuffer = static_cast<uint8_t*>(malloc(OUTFS_DECODE_BUFSIZE));
	}
}

OutputAAC::OutputAAC(const fs::File& file) : OutputAAC(){
	this->file = file;
}

OutputAAC::~OutputAAC(){
	for(auto& outBuffer : outBuffers){
		delete outBuffer;
	}

	free(decodeBuffer);
}

const fs::File& OutputAAC::getFile() const{
	return file;
}

void OutputAAC::setFile(const fs::File& file){
	OutputAAC::file = file;
}

static void* inBuffer[] = { inBuffer };
static INT inBufferIds[] = { IN_AUDIO_DATA };
static INT inBufferSize[] = { BUFFER_SAMPLES * NUM_CHANNELS };

static INT inBufferElSize[] = { BYTES_PER_SAMPLE };
static void* outBuffer[] = { nullptr };
static INT outBufferIds[] = { OUT_BITSTREAM_DATA };
static INT outBufferSize[] = { 0 };

static INT outBufferElSize[] = { sizeof(UCHAR) };

void OutputAAC::setupBuffers(){
	inBufDesc.numBufs = sizeof(::inBuffer) / sizeof(void*);
	inBufDesc.bufs = (void**) &::inBuffer;
	inBufDesc.bufferIdentifiers = inBufferIds;
	inBufDesc.bufSizes = inBufferSize;
	inBufDesc.bufElSizes = inBufferElSize;

	outBufDesc.numBufs = sizeof(outBuffer) / sizeof(void*);
	outBufDesc.bufs = (void**) &outBuffer;
	outBufDesc.bufferIdentifiers = outBufferIds;
	outBufDesc.bufSizes = outBufferSize;
	outBufDesc.bufElSizes = outBufferElSize;

	inArgs = {
			.numInSamples = BUFFER_SAMPLES,
			.numAncBytes = 0
	};
}

void OutputAAC::output(size_t numSamples){
	Profiler.start("AAC write process");
	processWriteJob();
	Profiler.end();

	while(freeBuffers.empty()){
		processWriteJob();
	}

	DataBuffer* buffer = outBuffers[freeBuffers.front()];

	Profiler.start("AAC encode");
	// TODO: move these three to constructor
	::inBuffer[0] = this->inBuffer;
	::outBuffer[0] = decodeBuffer;
	::outBufferSize[0] = OUTFS_DECODE_BUFSIZE;
	inArgs.numInSamples = numSamples * NUM_CHANNELS;
	AACENC_OutArgs outArgs;
	int status = aacEncEncode(encoder, &inBufDesc, &outBufDesc, &inArgs, &outArgs);
	if(status != AACENC_OK){
		Serial.printf("ENC: %x\n", status);
	}
	Profiler.end();

	if(outArgs.numOutBytes != 0){
		memcpy(buffer->writeData(), decodeBuffer, outArgs.numOutBytes);
		buffer->writeMove(outArgs.numOutBytes);
	}

	if(buffer->readAvailable() >= OUTFS_WRITESIZE){
		Profiler.start("AAC write add");
		addWriteJob();
		Profiler.start("AAC write add");
	}
}

void OutputAAC::init(){
	if(!file){
		Serial.println("Output file not open");
		return;
	}

	if(aacEncOpen(&encoder, 0x01, 1) != AACENC_OK){
		Serial.println("encoder create error");
		return;
	}

	if(aacEncoder_SetParam(encoder, AACENC_AOT, 2) != AACENC_OK) Serial.println("1 err");
	if(aacEncoder_SetParam(encoder, AACENC_SAMPLERATE, SAMPLE_RATE) != AACENC_OK) Serial.println("2 err");
	if(aacEncoder_SetParam(encoder, AACENC_CHANNELMODE, MODE_1) != AACENC_OK) Serial.println("4 err");
	if(aacEncoder_SetParam(encoder, AACENC_BITRATE,  64000) != AACENC_OK) Serial.println("3 err"); // 140000

	if(aacEncoder_SetParam(encoder, AACENC_TRANSMUX,  2) != AACENC_OK) Serial.println("5 err");
	if(aacEncoder_SetParam(encoder, AACENC_SIGNALING_MODE,  0) != AACENC_OK) Serial.println("6 err");
	if(aacEncoder_SetParam(encoder, AACENC_AFTERBURNER,  0) != AACENC_OK) Serial.println("7 err");
	// if(aacEncoder_SetParam(encoder, AACENC_SBR_MODE,  1) != AACENC_OK) Serial.println("8 err");
	// if(aacEncoder_SetParam(encoder, AACENC_SBR_RATIO,  1) != AACENC_OK) Serial.println("9 err");
	// if(aacEncoder_SetParam(encoder, AACENC_GRANULE_LENGTH,  128) != AACENC_OK) Serial.println("10 err");


	int ret;
	if(ret = aacEncEncode(encoder, nullptr, nullptr, nullptr, nullptr) != AACENC_OK){
		Serial.printf("encoder first run error %d\n", ret);
		return;
	}

	AACENC_InfoStruct info = {0};
	if(aacEncInfo(encoder, &info) != AACENC_OK) {
		Serial.println("encoder info error");
		return;
	}

	/*size_t outSize = max(8192, NUM_CHANNELS * 768);
	Serial.printf("FRAME LENGTH: %ld\n", outSize);*/

	setupBuffers();
	file.seek(0);
}

void OutputAAC::deinit(){
	if(!freeBuffers.empty() && outBuffers[freeBuffers.front()]->readAvailable() > 0){
		addWriteJob();
	}

	inArgs.numInSamples = -1;
	AACENC_OutArgs outArgs;
	int status = aacEncEncode(encoder, nullptr, &outBufDesc, &inArgs, &outArgs);
	if(status != AACENC_OK){
		Serial.printf("ENC flush: %d\n", status);
	}

	if(outArgs.numOutBytes != 0){
		while(freeBuffers.empty()){
			processWriteJob();
		}

		DataBuffer* buffer = outBuffers[freeBuffers.front()];
		memcpy(buffer->writeData(), decodeBuffer, outArgs.numOutBytes);
		buffer->writeMove(outArgs.numOutBytes);

		addWriteJob();
	}

	aacEncClose(&encoder);

	while(freeBuffers.size() != OUTFS_BUFCOUNT){
		processWriteJob();
	}
}

void OutputAAC::addWriteJob(){
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

void OutputAAC::processWriteJob(){
	for(int i = 0; i < OUTFS_BUFCOUNT; i++){
		if(!writePending[i]) continue;
		if(writeResult[i] == nullptr) continue;

		outBuffers[i]->clear();

		delete writeResult[i];
		writeResult[i] = nullptr;

		writePending[i] = false;
		freeBuffers.push_back(i);
	}
}
