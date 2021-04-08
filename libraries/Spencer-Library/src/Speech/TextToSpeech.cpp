#include <sstream>
#include <SerialFlash.h>
#include <Network/Net.h>
#include "TextToSpeech.h"
#include "../DataStream/FileWriteStream.h"
#include "../Util/Base64Decode.h"
#include "../Settings.h"

#define CA "DC:03:B5:D6:0C:F1:02:F1:B1:D0:62:27:9F:3E:B4:C3:CD:C9:93:BA:20:65:6D:06:DC:5D:56:AC:CC:BA:40:20"

const char* stash[] = {
		"recording-1.mp3",
		"recording-2.mp3",
		"recording-3.mp3",
		"recording-4.mp3"
};

const char* TTSStrings[] = { "OK", "not connected to a network", "buffer file error", "server response error", "server error", "file limit", "text limit" };

#define STASH_COUNT (sizeof(stash) / sizeof(stash[0]))
#define CHAR_LIMIT 130

TextToSpeechImpl TextToSpeech;

TTSResult::TTSResult(TTSError error) : error(error), filename(nullptr), size(0){ }

TextToSpeechImpl::TextToSpeechImpl() : AsyncProcessor("TTS_Task", STASH_COUNT), fileStash(::stash, ::stash + STASH_COUNT){

}

void TextToSpeechImpl::releaseRecording(const char* filename){
	stashMut.lock();
	fileStash.insert(filename);
	stashMut.unlock();
}

void TextToSpeechImpl::doJob(const TTSJob& job){
	if(job.text.length() > 130){
		*job.result = new TTSResult(TTSError::TEXTLIMIT);
		return;
	}

	if(!Net.checkConnection() && !Net.reconnect()){
		*job.result = new TTSResult(TTSError::NETWORK);
		return;
	}

	stashMut.lock();
	if(fileStash.size() == 0){
		Serial.println("TTS is limited to 4 simultaneous samples");
		stashMut.unlock();
		*job.result = new TTSResult(TTSError::FILELIMIT);
		return;
	}

	const char* filename = *fileStash.begin();
	fileStash.erase(filename);
	stashMut.unlock();

	*job.result = generateSpeech(job.text, filename);

	if((*job.result)->error != TTSError::OK){
		releaseRecording(filename);
	}
}

TTSResult* TextToSpeechImpl::generateSpeech(const std::string& text, const char* filename){
	const char pattern[] = "{ 'input': { 'text': '%.*s' },"
						   "'voice': {"
						   "'languageCode': 'en-US',"
						   "'name': 'en-US-Standard-D',"
						   "'ssmlGender': 'NEUTRAL'"
						   "}, 'audioConfig': {"
						   "'audioEncoding': 'MP3',"
						   "'speakingRate': 0.96,"
						   "'pitch': 5.5,"
						   "'sampleRateHertz': 16000"
						   "}}";

	char* data = (char*) malloc(sizeof(pattern) + (text.length() > CHAR_LIMIT ? CHAR_LIMIT : text.length()) + 2);
	uint length = sprintf(data, pattern, CHAR_LIMIT, text.c_str());

	StreamableHTTPClient http;
	http.useHTTP10(true);
	http.setReuse(false);
	if(!http.begin("https://spencer.circuitmess.com:8443/tts/v1/text:synthesize", CA)){
		free(data);
		return new TTSResult(TTSError::NETWORK);
	}
	http.addHeader("Content-Type", "application/json; charset=utf-8");
	http.addHeader("Accept-Encoding", "identity");
	http.addHeader("Content-Length", String(length));

	if(!http.startPOST()){
		Serial.println("Error connecting");
		http.end();
		http.getStream().stop();
		http.getStream().flush();
		free(data);
		return new TTSResult(TTSError::NETWORK);
	}

	if(!http.send(reinterpret_cast<uint8_t*>(data), length)){
		Serial.println("Error sending data");
		http.end();
		http.getStream().stop();
		http.getStream().flush();
		free(data);
		return new TTSResult(TTSError::NETWORK);
	}
	free(data);
	int code = http.finish();
	if(code != 200){
		Serial.printf("HTTP code %d\n", code);
		http.end();
		http.getStream().stop();
		http.getStream().flush();
		return new TTSResult(TTSError::JSON);
	}

	enum { PRE, PROP, VAL, POST } state = PRE;
	WiFiClient& stream = http.getStream();
	bool processed = false;
	size_t fileSize = 0;

	while(stream.connected()){
		if(state == PRE){
			readUntilQuote(stream);
			state = PROP;
		}else if(state == PROP){
			String prop = stream.readStringUntil('"');
			if(prop.equals("audioContent")){
				readUntilQuote(stream);
				state = VAL;
			}else{
				readUntilQuote(stream);
				readUntilQuote(stream);
				state = PRE;
			}
		}else if(state == VAL){
			if((fileSize = processStream(stream, filename)) < 0){
				return new TTSResult(TTSError::FILE);
			}else if(fileSize == 0){
				return new TTSResult(TTSError::JSON);
			}
			processed = true;
			break;
		}
	}

	http.end();
	stream.stop();
	stream.flush();

	if(!processed){
		Serial.println("Error processing stream");
		return new TTSResult(TTSError::JSON);
	}

	TTSResult* result = new TTSResult(TTSError::OK);
	result->filename = filename;
	result->size = fileSize;
	return result;
}

int TextToSpeechImpl::processStream(WiFiClient& stream, const char* filename){
	if(filename == nullptr) return -1;

	if(!SerialFlash.exists(filename)){
		SerialFlash.createErasable(filename, 64000);
	}

	SerialFlashFile file = SerialFlash.open(filename);
	if(!file) return -2;

	file.erase();

	FileWriteStream fileStream(file);
	Base64Decode decodeStream(&fileStream);
	uint32_t size = 0;
	unsigned char byte;
	while(stream.connected()){
		if(!stream.available()) continue;
		if(stream.read(&byte, 1) != 1) continue;
		if(byte == '"') break;
		if(byte == '\n' || byte == '\r') continue;
		size += decodeStream.write_return(byte);
	}
	fileStream.flush();
	file.close();
	return size;
}

void TextToSpeechImpl::readUntilQuote(WiFiClient& stream){
	unsigned char byte;
	while(stream.connected()){
		if(!stream.available()) continue;
		stream.read(&byte, 1);
		if(byte == '"') return;
	}
}
