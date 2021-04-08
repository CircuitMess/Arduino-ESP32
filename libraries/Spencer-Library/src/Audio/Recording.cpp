#include "Recording.h"
#include "Compression.h"
#include <SerialFlash.h>

RecordingImpl Recording;

RecordingImpl::RecordingImpl() : AsyncProcessor("Record_Task"){

}

void RecordingImpl::begin(I2S* i2s){
	this->i2s = i2s;
}

void RecordingImpl::doJob(const RecordJob& job){
	record();
	*job.resultFilename = "recording.wav";
}

void RecordingImpl::record(){
	if(i2s == nullptr) return;

	const uint32_t wavBufferSize = sizeof(int16_t) * i2sBufferSize / 4; // i2sBuffer is stereo by byte, wavBuffer is mono int16
	const uint32_t noReadings = (maxRecordTime * sampleRate * 4) / i2sBufferSize; // time * sampleRate * 4 bytes per sample (sample is int16_t, 2 channels)

	char* i2sBuffer = static_cast<char*>(malloc(i2sBufferSize));
	int16_t* wavBuffer = static_cast<int16_t*>(malloc(wavBufferSize));
	const uint wavFileSize = maxRecordTime * (float) sampleRate * 2.0f;

	SerialFlash.createErasable("recording.wav", wavFileSize + wavHeaderSize);
	SerialFlashFile file = SerialFlash.open("recording.wav");
	file.erase();
	file.seek(wavHeaderSize);

	uint16_t ampBuffer[avgBufferSize] = { 0 };
	uint16_t ampPointer = 0;
	uint16_t maxAmp = 0;
	bool underMax = false;
	uint underMaxTime = 0;
	uint32_t wavTotalWritten = 0;

	i2s->begin();

	for(int i = 0; i < noReadings; i++){
		i2s->Read(i2sBuffer, i2sBufferSize);

		for(int j = 0; j < i2sBufferSize; j += 4){
			int16_t sample = *(int16_t*) (&i2sBuffer[j + 2]) + 3705;
			wavBuffer[j / 4] = sample;

			// Don't record max first 0.5s
			if(wavTotalWritten < 0.5 * 2 * sampleRate) continue;

			ampBuffer[ampPointer++] = abs(sample);
			if(ampPointer == avgBufferSize){
				ampPointer = 0;
			}

			uint32_t sum = 0;
			for(int k = 0; k < avgBufferSize; k++) sum += ampBuffer[k];
			uint16_t avgLastN = sum / avgBufferSize;
			maxAmp = max(maxAmp, avgLastN);

			if(abs(sample) < (float) maxAmp * cutoffThreshold){
				if(!underMax){
					underMax = true;
					underMaxTime = millis();
				}
			}else if(underMax){
				underMax = false;
			}
		}

		file.write(wavBuffer, wavBufferSize);
		wavTotalWritten += wavBufferSize;
		if((wavTotalWritten > minRecordTime * sampleRate * 2)
		&& (wavTotalWritten > 0.5 * 2 * sampleRate + avgBufferSize*2) // don't cutoff if we didn't even begin measuring
		&& underMax && (millis() - underMaxTime) >= cutoffTime * 1000){
			// wavTotalWritten -= cutoffTime * sampleRate; // half of cutoff time
			break;
		}
	}

	file.seek(0);
	writeWavHeader(&file, wavTotalWritten);
	file.close();

	free(i2sBuffer);
	free(wavBuffer);

	i2s->stop();
	createCompressedFile("compressed.wav", wavTotalWritten);
}

void RecordingImpl::createCompressedFile(const char* outputFilename, size_t wavSize){
	SerialFlash.createErasable(outputFilename, maxRecordTime * (float) sampleRate * 2.0f + (float) wavHeaderSize);
	SerialFlashFile file = SerialFlash.open(outputFilename);
	file.erase();
	if(!file){
		Serial.println("Failed opening output file");
		return;
	}

	writeWavHeader(&file, wavSize);
	file.close();
}

void RecordingImpl::writeWavHeader(SerialFlashFile* file, int wavSize){
	unsigned char header[wavHeaderSize];
	unsigned int fileSizeMinus8 = wavSize + 44 - 8;
	header[0] = 'R';
	header[1] = 'I';
	header[2] = 'F';
	header[3] = 'F';
	header[4] = (byte) (fileSizeMinus8 & 0xFF);
	header[5] = (byte) ((fileSizeMinus8 >> 8) & 0xFF);
	header[6] = (byte) ((fileSizeMinus8 >> 16) & 0xFF);
	header[7] = (byte) ((fileSizeMinus8 >> 24) & 0xFF);
	header[8] = 'W';
	header[9] = 'A';
	header[10] = 'V';
	header[11] = 'E';
	header[12] = 'f';
	header[13] = 'm';
	header[14] = 't';
	header[15] = ' ';
	header[16] = 0x10;  // linear PCM
	header[17] = 0x00;
	header[18] = 0x00;
	header[19] = 0x00;
	header[20] = 0x01;  // linear PCM
	header[21] = 0x00;
	header[22] = 0x01;  // monoral
	header[23] = 0x00;
	header[24] = 0x80;  // sampling rate 16000
	header[25] = 0x3E;
	header[26] = 0x00;
	header[27] = 0x00;
	header[28] = 0x00;  // Byte/sec = 16000x2x1 = 32000
	header[29] = 0x7D;
	header[30] = 0x00;
	header[31] = 0x00;
	header[32] = 0x02;  // 16bit monoral
	header[33] = 0x00;
	header[34] = 0x10;  // 16bit
	header[35] = 0x00;
	header[36] = 'd';
	header[37] = 'a';
	header[38] = 't';
	header[39] = 'a';
	header[40] = (byte) (wavSize & 0xFF);
	header[41] = (byte) ((wavSize >> 8) & 0xFF);
	header[42] = (byte) ((wavSize >> 16) & 0xFF);
	header[43] = (byte) ((wavSize >> 24) & 0xFF);

	file->write(header, sizeof(header));
}
