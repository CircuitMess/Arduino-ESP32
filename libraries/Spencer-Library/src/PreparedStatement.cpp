#include <Loop/LoopManager.h>
#include "PreparedStatement.h"
#include "Audio/AudioFileSourceSerialFlash.h"
#include "Audio/silence.hpp"

PreparedStatement::~PreparedStatement(){
	for(const Part& part : parts){
		if(part.type != Part::TTS) continue;
		delete (std::string*) part.content;
	}

	for(TTSResult* result : TTSresults){
		if(result == nullptr) continue;
		TextToSpeech.releaseRecording(result->filename);
		delete result;
	}

	for(String* string : stringTTS){
		delete string;
	}
}

void PreparedStatement::addSample(AudioFileSource* sample){
	parts.push_back({ Part::SAMPLE, sample });
}

bool PreparedStatement::addTTS(const char* text){
	if(text == nullptr) return false;
	parts.push_back({ Part::TTS, new std::string(text) });
	return true;
}

bool PreparedStatement::addTTS(const String& text){
	if(text.length() == 0) return false;
	stringTTS.push_back(new String(text));
	addTTS(stringTTS.back()->c_str());
}

void PreparedStatement::loop(uint micros){
	for(const TTSResult* result : TTSresults){
		if(result == nullptr) return;
	}

	LoopManager::removeListener(this);

	CompositeAudioFileSource* source = new CompositeAudioFileSource();
	int i = 0;
	TTSError error = TTSError::OK;
	for(const Part& part : parts){
		if(part.type == Part::SAMPLE){
			source->add(static_cast<AudioFileSource*>(part.content));
		}else{
			if(TTSresults[i]->error != TTSError::OK){
				error = TTSresults[i]->error;
			}else{
				source->add(new AudioFileSourceSerialFlash(TTSresults[i]->filename, TTSresults[i]->size));
			}
			i++;
		}
	}

	if(playCallback != nullptr){
		playCallback(error, source);
	}else{
		delete source;
	}
}

void PreparedStatement::prepare(void (*playCallback)(TTSError error, CompositeAudioFileSource* source)){
	this->playCallback = playCallback;

	TTSresults.reserve(parts.size());

	for(const Part& part : parts){
		if(part.type == Part::TTS){
			TTSresults.push_back(nullptr);
			TextToSpeech.addJob({ * (std::string*) part.content, &TTSresults.back() });
		}
	}

	TTSresults.shrink_to_fit();

	/**
	 * Weird bug happening when PreparedStatement consists only of TTS parts: every three used samples, 30kb
	 * of heap storage will be lost. Acquired by something and never released. Traced it down, and it's 100%
	 * not a leak in Spencer's library.
	 *
	 * The fix is to add a single empty mp3 audio sample.
	 */
	if(TTSresults.size() == parts.size()){
		SerialFlashFile file;

		if(!SerialFlash.exists("silence.mp3")){
			SerialFlash.create("silence.mp3", sizeof(silence));
			file = SerialFlash.open("silence.mp3");

			uint8_t buffer[sizeof(silence)];
			memcpy_P(buffer, silence, sizeof(silence));
			file.write(buffer, sizeof(silence));

			file.close();
		}

		parts.insert(parts.begin(), { Part::SAMPLE, new AudioFileSourceSerialFlash("silence.mp3") });
	}

	LoopManager::addListener(this);
}
