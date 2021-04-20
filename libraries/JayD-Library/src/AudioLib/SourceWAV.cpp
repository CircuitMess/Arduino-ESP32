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


SourceWAV::SourceWAV() :
#ifdef BUFFER_COUNT
readBuffer(BUFFER_COUNT * BUFFER_SIZE)
#else
readBuffer(BUFFER_SIZE)
#endif
{

}

SourceWAV::SourceWAV(fs::File file) : SourceWAV(){
	open(file);
}

void SourceWAV::open(fs::File file){
	this->file = file;
	channels = sampleRate = bytesPerSample = 0;
	readHeader();
	readBuffer.clear();
	addReadJob(true);
}

SourceWAV::~SourceWAV(){

}

void SourceWAV::addReadJob(bool full){
	if(readJobPending) return;

	delete readResult;
	readResult = nullptr;

	size_t size = full ? readBuffer.writeAvailable() : readChunkSize;

	//Serial.printf("Adding read job, size: %ld\n", size);

	if(size == 0 || readBuffer.writeAvailable() < size){
		return;
	}

	uint8_t* buf;
	if(size <= readChunkSize || !psramFound()){
		buf = static_cast<uint8_t*>(malloc(size));
	}else{
		buf = static_cast<uint8_t*>(ps_malloc(size));
	}

	Sched.addJob(new SDJob{
			 .type = SDJob::SD_READ,
			 .file = file,
			 .size = size,
			 .buffer = buf,
			 .result = &readResult
	 });

	readJobPending = true;
}

void SourceWAV::processReadJob(){
	if(readResult == nullptr){
		if(readBuffer.readAvailable() < BUFFER_SIZE){
			Profiler.start("WAV read wait");
			while(readResult == nullptr){
				delayMicroseconds(1);
			}
			Profiler.end();
		}else{
			return;
		}
	}

	/*if(readBuffer.readAvailable() < BUFFER_SIZE){
		// chunk size is smaller than BUFFER_SIZE
		// lets recurse this shit

		addReadJob();
		processReadJob();
		return;
	}*/

	// Serial.printf("Processing read job, buffer size: %ld\n", readResult->size);

	readBuffer.write(readResult->buffer, readResult->size);
	free(readResult->buffer);

	delete readResult;
	readResult = nullptr;

	readJobPending = false;
}

size_t SourceWAV::generate(int16_t *outBuffer){
	if(!file){
		Serial.println("file false");
		return 0;
	}

	if(sampleRate == 0 || channels == 0 || bytesPerSample == 0){
		if(!readHeader()) return 0;
	}

	processReadJob();

	//Serial.printf("Playing, available %ld, took %ld\n", readBuffer.readAvailable(), BUFFER_SIZE);

	size_t size = readBuffer.read(reinterpret_cast<uint8_t*>(outBuffer), BUFFER_SIZE);
	size_t samples = size / (NUM_CHANNELS * BYTES_PER_SAMPLE);

	for(int i = 0; i < samples; i++){
		outBuffer[i] *= volume;
	}

	addReadJob();

	readData += size;
	return samples;
}

bool SourceWAV::readHeader(){
	if(!file){
		Serial.println("file false");
		return false;
	}

	file.seek(0);

	char *buffer = (char*)malloc(sizeof(WavHeader));
	if(file.readBytes(buffer, sizeof(WavHeader)) != sizeof(WavHeader)){
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
	return (file.available()/(channels*bytesPerSample));
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
	size_t offset = time*sampleRate*channels*bytesPerSample;
	if(offset >= file.size()) return;

	file.seek(offset, mode);
}

void SourceWAV::close(){

}

void SourceWAV::setVolume(uint8_t volume){
	SourceWAV::volume = (float) volume / 255.0f;
}
