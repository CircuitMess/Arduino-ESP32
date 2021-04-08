#ifndef SPENCER_SPEECH2INTENT_H
#define SPENCER_SPEECH2INTENT_H

#include <queue>
#include <Sync/BinarySemaphore.h>
#include <Sync/Mutex.h>
#include <Util/Task.h>
#include <string>
#include <map>
#include "../AsyncProcessor.hpp"

extern const char* STIStrings[];

struct IntentResult {
	enum Error { OK = 0, NETWORK, FILE, JSON, INTENT, KEY } error;
	char* transcript;
	char* intent;
	float confidence;
	std::map<std::string, std::string> entities;

	IntentResult(Error error);
	virtual ~IntentResult();
};

struct STIJob {
	const char* recordingFilename;
	IntentResult** result;
};

class SpeechToIntentImpl : public AsyncProcessor<STIJob> {
public:
	SpeechToIntentImpl();

protected:
	void doJob(const STIJob& job) override;

private:
	IntentResult* identifyVoice(const char* filename);
	void compress(const char* inputFilename, const char* outputFilename);

};

extern SpeechToIntentImpl SpeechToIntent;
#endif