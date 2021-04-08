#ifndef SPENCER_RECORDING_H
#define SPENCER_RECORDING_H


#include "I2S.h"
#include "../AsyncProcessor.hpp"

class SerialFlashFile;

struct RecordJob {
	const char** resultFilename;
};

class RecordingImpl : public AsyncProcessor<RecordJob> {
public:
	RecordingImpl();

	void begin(I2S* i2s);

protected:
	void doJob(const RecordJob& job) override;

private:
	const uint16_t wavHeaderSize = 44;
	const uint32_t i2sBufferSize = 1600;
	const uint32_t sampleRate = 16000;
	const float minRecordTime = 2.0f; // in seconds
	const float maxRecordTime = 3.5f; // in seconds
	const float cutoffThreshold = 0.3f;
	const float cutoffTime = 1.0f; // in seconds, time where amplitude is below max*cutoffThreshold before recording is cut
#define avgBufferSize 10

	I2S* i2s = nullptr;

	void record(); // 16bit, monoral, 16000Hz,  linear PCM

	void writeWavHeader(SerialFlashFile* file, int wavSize);
	void createCompressedFile(const char* outputFilename, size_t wavSize);
};

extern RecordingImpl Recording;

#endif //SPENCER_RECORDING_H
