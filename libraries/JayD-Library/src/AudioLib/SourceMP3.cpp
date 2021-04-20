#include "SourceMP3.h"
#include "../PerfMon.h"
#include "Decoder/ID3Parser.h"

#define MP3_READ_BUFFER 1024 * 64
#define MP3_READ_CHUNK 1024 * 4 // should be bigger than min input
#define MP3_DECODE_MIN_INPUT 1024 * 2 // should be smaller than read chunk
#define MP3_OUT_BUFFER 1024 * 8

SourceMP3::SourceMP3() :
		readBuffer(MP3_READ_BUFFER),
		fillBuffer(MP3_DECODE_MIN_INPUT),
		dataBuffer(MP3_OUT_BUFFER, true){

}

SourceMP3::SourceMP3(fs::File file) : SourceMP3(){
	open(file);
}

void SourceMP3::open(fs::File file){
	this->file.close();
	channels = sampleRate = bytesPerSample = bitrate = readData = 0;
	readBuffer.clear();
	dataBuffer.clear();

	if(!file){
		return;
	}

	ID3Parser parser(file);
	metadata = parser.parse();
	if(metadata.headerSize == 0){
		Serial.println("Failed parsing MP3 header");
		return;
	}

	dataSize = file.size();
	bitrate = 128000;

	decoder = MP3InitDecoder();
	if(decoder == nullptr){
		Serial.println("Decoder construct fail");

	}else{
		Serial.println("Decoder constructed");
	}

	file.seek(metadata.headerSize);
	this->file = file;
	addReadJob(true);
}

SourceMP3::~SourceMP3(){

}

void SourceMP3::addReadJob(bool full){
	if(readJobPending) return;

	delete readResult;
	readResult = nullptr;

	size_t size = full ? readBuffer.writeAvailable() : MP3_READ_CHUNK;

	//Serial.printf("Adding read job, size: %ld\n", size);

	if(size == 0 || readBuffer.writeAvailable() < size){
		return;
	}

	uint8_t* buf;
	if(size <= MP3_READ_CHUNK || !psramFound()){
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

void SourceMP3::processReadJob(){
	if(readResult == nullptr){
		if(readBuffer.readAvailable() + fillBuffer.readAvailable() < MP3_DECODE_MIN_INPUT){
			Serial.println("small");
			while(readResult == nullptr){
				delayMicroseconds(1);
			}
			Serial.println("ok");
		}else{
			return;
		}
	}

	readBuffer.write(readResult->buffer, readResult->size);
	free(readResult->buffer);

	delete readResult;
	readResult = nullptr;

	readJobPending = false;
}

size_t SourceMP3::generate(int16_t* outBuffer){
	if(!file){
		Serial.println("file false");
		return 0;
	}

	if(!decoder){
		Serial.println("Decoder false");
		return 0;
	}

	Profiler.start("MP3 read job process");
	processReadJob();
	Profiler.end();

	Profiler.start("decode");
	while(dataBuffer.readAvailable() < BUFFER_SIZE){
		// Serial.printf("Grabbing, available %ld, taking %ld\n", readBuffer.readAvailable(), fillBuffer.writeAvailable());
		//Profiler.start("fill read");
		fillBuffer.writeMove(readBuffer.read(fillBuffer.writeData(), fillBuffer.writeAvailable()));
		if(fillBuffer.readAvailable() < MP3_DECODE_MIN_INPUT){
			Serial.println("rezerva");
			addReadJob();
			processReadJob();
			fillBuffer.writeMove(readBuffer.read(fillBuffer.writeData(), fillBuffer.writeAvailable()));
		}
		//Profiler.end();

		int frameOffset = MP3FindSyncWord(fillBuffer.readData(), fillBuffer.readAvailable());
		if(frameOffset == -1){
			Serial.println("Can't find frame");
			return 0;
		}
		fillBuffer.readMove(frameOffset);


		uint frameSize = 0; //adts->frame_length_0_to_1 << 11 | adts->frame_length_2_to_9 << 3 | adts->frame_length_10_to_12;

		uint8_t* data = const_cast<uint8_t*>(fillBuffer.readData());
		int bytesLeft = fillBuffer.readAvailable();
		// Serial.printf("Decoding, available %ld\n", fillBuffer.readAvailable());
		int ret = MP3Decode(decoder, &data, &bytesLeft, reinterpret_cast<short*>(dataBuffer.writeData()), 0);
		if(ret){
			Serial.printf("decode error %d, frame size %d B\n", ret, frameSize);
			fillBuffer.readMove(frameSize);
			continue;
		}

		readData += fillBuffer.readAvailable() - bytesLeft;
		fillBuffer.readMove(fillBuffer.readAvailable() - bytesLeft);

		MP3FrameInfo fi;
		MP3GetLastFrameInfo(decoder, &fi);

		sampleRate = fi.samprate;
		channels = fi.nChans;
		bytesPerSample = 2;

		// Serial.printf("srate %d, channels %d, bps %d\n", sampleRate, channels, fi.bitsPerSample);

		int16_t* samples = (int16_t*) dataBuffer.readData();

		if(channels > 1){
			for(int i = 0; i < (fi.outputSamps); i += channels){
				int16_t  v = 0;
				for(int j = 0; j < channels; j++){
					samples[i/channels] += samples[i+j] / channels;
				}
				samples[i/channels] = v;
			}
		}

		dataBuffer.writeMove(fi.outputSamps * bytesPerSample);
	}
	Profiler.end();

	size_t size = min((size_t) BUFFER_SIZE, dataBuffer.readAvailable());
	memcpy(outBuffer, dataBuffer.readData(), size);
	dataBuffer.readMove(size);
	size_t samples = size / (NUM_CHANNELS * BYTES_PER_SAMPLE);

	for(int i = 0; i < samples; i++){
		outBuffer[i] *= volume;
	}

	Profiler.start("AAC read job add");
	addReadJob();
	Profiler.end();

	return samples;
}

int SourceMP3::available(){
	if(sampleRate == 0 || channels == 0 || bytesPerSample == 0) return 0;
	return (file.available() / (channels * bytesPerSample));
}

uint16_t SourceMP3::getDuration(){
	if(bitrate == 0) return 0;
	return 8 * dataSize / bitrate;
}

uint16_t SourceMP3::getElapsed(){
	if(bitrate == 0) return 0;
	return readData / SAMPLE_RATE;
}

void SourceMP3::seek(uint16_t time, fs::SeekMode mode){
	if(sampleRate == 0 || channels == 0 || bytesPerSample == 0) return;
	size_t offset = time * sampleRate * channels * bytesPerSample;
	if(offset >= file.size()) return;

	file.seek(offset, mode);
}

void SourceMP3::close(){

}

void SourceMP3::setVolume(uint8_t volume){
	SourceMP3::volume = (float) volume / 255.0f;
}

const ID3Metadata& SourceMP3::getMetadata() const{
	return metadata;
}

