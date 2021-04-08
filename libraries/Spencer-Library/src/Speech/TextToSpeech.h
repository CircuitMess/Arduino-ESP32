#ifndef SPENCER_TEXT2SPEECH_H
#define SPENCER_TEXT2SPEECH_H

#include <HTTPClient.h>
#include <set>
#include <Network/StreamableHTTPClient.h>
#include "../AsyncProcessor.hpp"

extern const char* TTSStrings[];

enum class TTSError { OK = 0, NETWORK, FILE, JSON, KEY, FILELIMIT, TEXTLIMIT };

struct TTSResult {
	TTSResult(TTSError error);

	const char* filename;
	TTSError error;
	size_t size;
};

struct TTSJob {
	std::string text;
	TTSResult** result;
};
class TextToSpeechImpl : public AsyncProcessor<TTSJob> {
public:
	TextToSpeechImpl();

	void releaseRecording(const char* filename);

protected:
	void doJob(const TTSJob& job) override;

private:
	TTSResult* generateSpeech(const std::string& text, const char* filename = "speech.mp3");
	int processStream(WiFiClient& stream, const char* filename);
	void readUntilQuote(WiFiClient& stream);

	std::set<const char*> fileStash;
	Mutex stashMut;
};

extern TextToSpeechImpl TextToSpeech;
#endif