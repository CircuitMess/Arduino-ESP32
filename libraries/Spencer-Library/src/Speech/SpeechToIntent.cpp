#include <SerialFlash.h>
#include <ArduinoJson.h>
#include "SpeechToIntent.h"
#include "../Util/Base64Encode.h"
#include <Network/StreamableHTTPClient.h>
#include <Network/Net.h>
#include "../Settings.h"
#include "../Audio/Compression.h"

#define CA "DC:03:B5:D6:0C:F1:02:F1:B1:D0:62:27:9F:3E:B4:C3:CD:C9:93:BA:20:65:6D:06:DC:5D:56:AC:CC:BA:40:20"

const char* STIStrings[] = { "OK", "not connected to a network", "buffer file error", "server response error", "no intent detected", "server error" };

SpeechToIntentImpl SpeechToIntent;

IntentResult::IntentResult(Error error) : error(error), entities({}){
	intent = transcript = nullptr;
	confidence = 0;
}

IntentResult::~IntentResult(){
	delete transcript;
	delete intent;
}

SpeechToIntentImpl::SpeechToIntentImpl() : AsyncProcessor("STI_Job"){

}

void SpeechToIntentImpl::doJob(const STIJob& job){
	if(!Net.checkConnection() && !Net.reconnect()){
		*job.result = new IntentResult(IntentResult::NETWORK);
		return;
	}

	IntentResult* result = identifyVoice(job.recordingFilename);

	*job.result = result;
}

IntentResult* SpeechToIntentImpl::identifyVoice(const char* filename){
	compress(filename, "compressed.wav");

	SerialFlashFile file = SerialFlash.open("compressed.wav");
	if(!file){
		Serial.println("Couldn't open file for reading");
		return new IntentResult(IntentResult::FILE);
	}

	uint wavSize = 0;
	file.seek(4); //skip RIFF on start of file
	for(int8_t i = 0; i < 4; i++){
		file.read(&(((char*)(void*)&wavSize)[i]), 1);
	}
	wavSize += 8 + 44;
	file.seek(0);

	StreamableHTTPClient http;
	http.useHTTP10(true);
	http.setReuse(false);
	if(!http.begin("https://spencer.circuitmess.com:8443/sti/speech", CA)){
		return new IntentResult(IntentResult::NETWORK);
	}

	http.addHeader("Content-Type", "audio/wav");
	http.addHeader("Accept-Encoding", "identity");
	http.addHeader("Content-Length", String(wavSize));


	if(!http.startPOST()){
		Serial.println("Error connecting");
		http.end();
		http.getStream().stop();
		http.getStream().flush();
		return new IntentResult(IntentResult::NETWORK);
	}

	uint sent = 0;
	unsigned char buffer[256];
	while(sent < wavSize){
		uint readSize = min((uint) 256, wavSize - sent);
		file.read(buffer, readSize);
		if(!http.send(buffer, readSize)){
			http.end();
			http.getStream().stop();
			http.getStream().flush();
			return new IntentResult(IntentResult::NETWORK);
		}
		sent += readSize;
	}


	int code = http.finish();
	if(code != 200){
		Serial.printf("HTTP code %d\n", code);
		http.end();
		http.getStream().stop();
		http.getStream().flush();
		return new IntentResult(IntentResult::JSON);
	}

	const int SIZE = JSON_ARRAY_SIZE(2) + JSON_OBJECT_SIZE(50) + 200;
	DynamicJsonDocument json(SIZE);
	DeserializationError error = deserializeJson(json, http.getStream());

	http.end();
	http.getStream().stop();
	http.getStream().flush();

	if(error){
		Serial.print(F("Parsing JSON failed: "));
		Serial.println(error.c_str());
		return new IntentResult(IntentResult::JSON);
	}

	if(!json.containsKey("text")){
		return new IntentResult(IntentResult::INTENT);
	}

	IntentResult* result = new IntentResult(IntentResult::OK);

	const char* transcript = json["text"].as<const char*>();
	uint32_t transcriptLength = strlen(transcript);
	result->transcript = static_cast<char*>(malloc(transcriptLength + 1));
	memset(result->transcript, 0, transcriptLength+1);
	memcpy(result->transcript, transcript, transcriptLength);

	const char* intent;
	if(!json.containsKey("intents") || json["intents"].size() == 0 || (intent = json["intents"][0]["name"].as<const char*>())[0] == '\0'){
		result->intent = nullptr;
		result->confidence = 1;
		result->error = IntentResult::INTENT;
		return result;
	}

	result->confidence = json["intents"][0]["confidence"].as<float>();

	uint32_t intentLength = strlen(intent);
	result->intent = static_cast<char*>(malloc(intentLength + 1));
	memset(result->intent, 0, intentLength+1);
	memcpy(result->intent, intent, intentLength);

	if(!json.containsKey("entities")) return result;

	for(const auto& entityObj : json["entities"].as<JsonObject>()){
		auto entityArr = entityObj.value().as<JsonArray>();
		if(entityArr.size() == 0) continue;
		auto entity = entityArr[0];

		if(!entity.containsKey("name") || !entity.containsKey("body")) continue;

		std::string slot = entity["name"].as<std::string>();
		std::string value = entity["body"].as<std::string>();

		if(result->entities.find(slot) != result->entities.end()) continue;

		result->entities.insert(std::pair<std::string, std::string>(slot, value));
	}

	return result;
}

void SpeechToIntentImpl::compress(const char* inputFilename, const char* outputFilename){
	SerialFlashFile input = SerialFlash.open(inputFilename);
	if(!input){
		Serial.println("Failed opening input file");
		return;
	}

	SerialFlashFile output = SerialFlash.open(outputFilename);
	if(!output){
		Serial.println("Failed opening output file");
		return;
	}

	uint wavSize = 0;
	input.seek(4); //skip RIFF on start of file
	for(int8_t i = 0; i < 4; i++){
		input.read(&(((char*)(void*)&wavSize)[i]), 1);
	}
	wavSize += 8;
	wavSize -= 44;
	input.seek(44); // wavHeaderSize
	output.seek(44); // wavHeaderSize

	const uint16_t samplesPerProcess = 32 * 100;

	Compression comp(16000, 10, 5, -26, 5, 5, 0.003f, 0.250f);
	int16_t* inputBuf = static_cast<int16_t*>(malloc(sizeof(int16_t) * samplesPerProcess));
	int16_t* outputBuf = static_cast<int16_t*>(malloc(sizeof(int16_t) * samplesPerProcess));

	size_t totalProcessed = 0;
	while(input.read(inputBuf, samplesPerProcess * sizeof(int16_t))){
		comp.process(inputBuf, outputBuf, samplesPerProcess);
		output.write(outputBuf, samplesPerProcess * sizeof(int16_t));

		totalProcessed += samplesPerProcess * sizeof(int16_t);
		if(totalProcessed >= wavSize) break;
	}

	input.close();
	output.close();
	free(inputBuf);
	free(outputBuf);
}