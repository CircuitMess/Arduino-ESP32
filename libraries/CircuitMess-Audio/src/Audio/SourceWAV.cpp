#include "SourceWAV.h"
#include "../PerfMon.h"

// Number of system buffers to load at once
#define BUFFER_COUNT 1024

//-------------------------------------------
// Helper structs and funcs
struct WavHeader{
	char RIFF[4];
	uint32_t chunkSize;
	char WAVE[4];
	char fmt[4];
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

// ----------------------------------------------


SourceWAV::SourceWAV(DataSource& ds) : Source(ds)
{
	channels = sampleRate = bytesPerSample = 0;
	readHeader();
}

size_t SourceWAV::generate(int16_t *outBuffer){

	if(sampleRate == 0 || channels == 0 || bytesPerSample == 0){
		if(!readHeader()) return 0;
	}

//	processReadJob();

	//Serial.printf("Playing, available %ld, took %ld\n", readBuffer.readAvailable(), BUFFER_SIZE);

	size_t size = ds.read(reinterpret_cast<uint8_t*>(outBuffer), BUFFER_SIZE);
	size_t samples = size / (NUM_CHANNELS * BYTES_PER_SAMPLE);

	for(int i = 0; i < samples; i++){
		outBuffer[i] *= volume;
	}

//	addReadJob();

	readData += size;
	return samples;
}

bool SourceWAV::readHeader(){

	ds.seek(0);

	auto buffer = (uint8_t*)malloc(sizeof(WavHeader));
	if(ds.read(buffer, sizeof(WavHeader)) != sizeof(WavHeader)){
		Serial.println("Error, couldn't read from file");
		free(buffer);
		return false;
	}
	WavHeader *header = (WavHeader*)buffer;
	if(memcmp(header->RIFF, "RIFF", 4) != 0){
		Serial.println("Error, couldn't find RIFF ID");
		free(buffer);
		return false;
	}
	if(memcmp(header->WAVE, "WAVE", 4) != 0){
		Serial.println("Error, couldn't find WAVE ID");
		free(buffer);
		return false;
	}
	if(memcmp(header->fmt, "fmt ", 4) != 0){
		Serial.println("Error, couldn't find fmt ID");
		free(buffer);
		return false;
	}
	if(memcmp(header->data, "data", 4) != 0){
		Serial.println("Error, couldn't find data ID");
		free(buffer);
		return false;
	}
	channels = header->numChannels;
	sampleRate = header->sampleRate;
	bytesPerSample = header->bitsPerSample / 8;
	dataSize = header->dataSize;
	free(buffer);
	return true;
}

int SourceWAV::available(){
	if(sampleRate == 0 || channels == 0 || bytesPerSample == 0) return 0;
	return (ds.available()/(channels*bytesPerSample));
}

uint16_t SourceWAV::getDuration(){
	if(sampleRate == 0 || channels == 0 || bytesPerSample == 0) return 0;
	return int(dataSize/(channels*bytesPerSample*sampleRate));
}

uint16_t SourceWAV::getElapsed(){
	if(sampleRate == 0 || channels == 0 || bytesPerSample == 0 || readData == 0) return 0;
	return int(readData/(channels*bytesPerSample*sampleRate));
}

void SourceWAV::seek(uint16_t time, fs::SeekMode mode){
	if(sampleRate == 0 || channels == 0 || bytesPerSample == 0 ) return;
	size_t offset = time*sampleRate*channels*bytesPerSample + sizeof(WavHeader);

	ds.seek(offset, mode);
}

void SourceWAV::close(){

}
