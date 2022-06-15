#include "SourceMP3.h"
#include "../PerfMon.h"
#include "Decoder/ID3Parser.h"

#define MP3_READ_BUFFER 1024 * 64
#define MP3_READ_CHUNK 1024 * 4 // should be bigger than min input
#define MP3_DECODE_MIN_INPUT 1024 * 2 // should be smaller than read chunk
#define MP3_OUT_BUFFER 1024 * 8

SourceMP3::SourceMP3(DataSource& ds) : Source(ds),
		fillBuffer(MP3_DECODE_MIN_INPUT),
		dataBuffer(MP3_OUT_BUFFER, true){

	channels = sampleRate = bytesPerSample = bitrate = readData = 0;
	dataBuffer.clear();

	ID3Parser parser(ds);
	metadata = parser.parse();
	if(metadata.headerSize == 0){
		Serial.println("Failed parsing MP3 header");
		return;
	}

	bitrate = 128000;

	decoder = MP3InitDecoder();
	if(decoder == nullptr){
		Serial.println("Decoder construct fail");

	}else{
		Serial.println("Decoder constructed");
	}

	ds.seek(metadata.headerSize);
//	addReadJob(true);
}

size_t SourceMP3::generate(int16_t* outBuffer){
	if(!decoder){
		Serial.println("Decoder false");
		return 0;
	}

	Profiler.start("MP3 read job process");
//	processReadJob();
	refill();
	Profiler.end();

	Profiler.start("decode");
	while(dataBuffer.readAvailable() < BUFFER_SIZE){
		// Serial.printf("Grabbing, available %ld, taking %ld\n", readBuffer.readAvailable(), fillBuffer.writeAvailable());
		//Profiler.start("fill read");
//		fillBuffer.writeMove(readBuffer.read(fillBuffer.writeData(), fillBuffer.writeAvailable()));
		if(fillBuffer.readAvailable() < MP3_DECODE_MIN_INPUT){
			Serial.println("rezerva");
//			addReadJob();
//			processReadJob();
//			fillBuffer.writeMove(readBuffer.read(fillBuffer.writeData(), fillBuffer.writeAvailable()));
			refill();
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

//	Profiler.start("AAC read job add");
//	addReadJob();
//	Profiler.end();

	return samples;
}

int SourceMP3::available(){
	if(sampleRate == 0 || channels == 0 || bytesPerSample == 0) return 0;
	return (ds.available() / (channels * bytesPerSample));
}

uint16_t SourceMP3::getDuration(){
	if(bitrate == 0) return 0;
	return 8 * ds.size() / bitrate;
}

uint16_t SourceMP3::getElapsed(){
	if(bitrate == 0) return 0;
	return readData / SAMPLE_RATE;
}

void SourceMP3::seek(uint16_t time, fs::SeekMode mode){
	if(sampleRate == 0 || channels == 0 || bytesPerSample == 0) return;
	size_t offset = time * sampleRate * channels * bytesPerSample;

	ds.seek(offset, mode);
}

void SourceMP3::close(){

}


const ID3Metadata& SourceMP3::getMetadata() const{
	return metadata;
}

void SourceMP3::refill(){
	size_t size = min(fillBuffer.writeAvailable(), ds.available());
	size = ds.read(fillBuffer.writeData(), size);
	fillBuffer.writeMove(size);
}

