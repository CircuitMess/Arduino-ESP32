#include "SourceAAC.h"
#include "../PerfMon.h"
#include "../Setup.hpp"

SourceAAC::SourceAAC(DataSource &ds) : Source(ds), fillBuffer(AAC_DECODE_MIN_INPUT), dataBuffer(AAC_OUT_BUFFER){
	channels = sampleRate = bitrate = movedBytes = 0;
	bytesPerSample = 2;

	dataBuffer.clear();
	fillBuffer.clear();

	dataSize = ds.available();
	bitrate = 64000;

	hAACDecoder = AACInitDecoder();
	if(hAACDecoder == nullptr){
		Serial.println("Decoder construct fail");

	}
}

SourceAAC::~SourceAAC(){
	SourceAAC::close();
}

void SourceAAC::setSongDoneCallback(void (*callback)()) {
	songDoneCallback = callback;
}

void SourceAAC::close(){

	channels = sampleRate = bytesPerSample = bitrate = movedBytes = 0;
	dataBuffer.clear();
	fillBuffer.clear();

	if(hAACDecoder){
		AACFreeDecoder(hAACDecoder);
		hAACDecoder = nullptr;
	}
}

int SourceAAC::available(){
	if(channels == 0 || bytesPerSample == 0 ) return 0;
	return (ds.available() / (channels * bytesPerSample));
}

size_t SourceAAC::generate(int16_t* outBuffer){
	if(!hAACDecoder){
		Serial.println("Decoder false");
		return 0;
	}

	Profiler.start("AAC read job process");
	refill();
	Profiler.end();

	if(fillBuffer.readAvailable() < AAC_DECODE_MIN_INPUT){
		Serial.println("if fillbuffer < aac_decode_min");
//		if(songDoneCallback != nullptr) {
//			songDoneCallback();
//		}
		if(repeat){
			ds.seek(0);
			refill();
		}else{
			return 0;
		}
	}

	while(dataBuffer.readAvailable() < BUFFER_SIZE){

		if(fillBuffer.readAvailable() < AAC_DECODE_MIN_INPUT){
			refill();
		}

		auto data = const_cast<uint8_t*>(fillBuffer.readData());
		int bytesLeft = fillBuffer.readAvailable();
		// Serial.printf("Decoding, available %ld\n", fillBuffer.readAvailable());
		int ret = AACDecode(hAACDecoder, &data, &bytesLeft, reinterpret_cast<short*>(dataBuffer.writeData()));
		if(ret){
			size_t frameSize = fillBuffer.readAvailable() - bytesLeft;
			Serial.printf("decode error %d, frame size %d B\n", ret, frameSize);
			movedBytes++;
			fillBuffer.readMove(1);
			continue;
		}
		movedBytes += fillBuffer.readAvailable() - bytesLeft;
		fillBuffer.readMove(fillBuffer.readAvailable() - bytesLeft);

		AACFrameInfo fi;
		AACGetLastFrameInfo(hAACDecoder, &fi);

		sampleRate = fi.sampRateOut;
		channels = fi.nChans;

		dataBuffer.writeMove(fi.outputSamps * channels * bytesPerSample);
	}
	Profiler.end();

	size_t size = min((size_t) BUFFER_SIZE, dataBuffer.readAvailable());
	memcpy(outBuffer, dataBuffer.readData(), size);
	dataBuffer.readMove(size);
	size_t samples = size / (NUM_CHANNELS * BYTES_PER_SAMPLE);

	for(int i = 0; i < samples; i++){
		outBuffer[i] *= volume;
	}

/*	if(samples == 0){
		ds.seek(0);

		if(songDoneCallback != nullptr) {
			songDoneCallback();
		}
		if(repeat){
			return generate(outBuffer);
		}
	}*/

	return samples;
}

void SourceAAC::refill(){
	size_t size = min(fillBuffer.writeAvailable(), ds.available());
	size = ds.read(fillBuffer.writeData(), size);
	fillBuffer.writeMove(size);
}

uint16_t SourceAAC::getDuration(){
	if(bitrate == 0) return 0;
	return 8 * dataSize / bitrate;
}

uint16_t SourceAAC::getElapsed(){
	if(bitrate == 0) return 0;
	return 8 * movedBytes / bitrate;
}

void SourceAAC::seek(uint16_t time, fs::SeekMode mode){
	size_t offset = time * bitrate / 8;
	if(offset >= dataSize){
		return;
	}
	ds.seek(offset);
	movedBytes = offset;
	resetDecoding();
}

void SourceAAC::resetDecoding() {
	dataBuffer.clear();
	fillBuffer.clear();
	AACFlushCodec(hAACDecoder);
}
