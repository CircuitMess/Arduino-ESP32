#include "AudioSystem.h"
#include <SPIFFS.h>
#include "Pins.hpp"

AudioSystem::AudioSystem() : task("Playback", [](Task* task){
	auto system = static_cast<AudioSystem*>(task->arg);
	system->player();
}, 4096, this), output(PIN_SPKR, PIN_SPKR_EN), playQueue(4, sizeof(PlayJob)){

	output.setSource(source);
}

AudioSystem::~AudioSystem(){
	stop();
}

void AudioSystem::begin(){
	if(task.running) return;

	task.start(1, 0);
}

void AudioSystem::stop(){
	task.stop(true);
	delete source;
	delete dataSource;
	source = nullptr;
	dataSource = nullptr;
}

void AudioSystem::playRepeating(File file) {
    auto dataSource = new FileDataSource(file);

    PlayJob job = { dataSource, true };
    playQueue.send(&job);

    running = true;
}

void AudioSystem::play(File file){
	auto dataSource = new FileDataSource(file);

	PlayJob job = { dataSource, false };
	playQueue.send(&job);

	running = true;
}

void AudioSystem::player(){
	while(task.running){
		while(playQueue.count()){
			PlayJob next;
			if(!playQueue.receive(&next)) continue;

			if(source){
				source->close();
			}

			delete source;
			delete dataSource;

			dataSource = next.dataSource;
			source = new SourceAAC(*dataSource);
            source->setRepeat(next.repeating);
			output.setSource(source);
			output.start();
		}

		bool wasRunning = output.isRunning();
		output.loop(0);
		if(wasRunning && !output.isRunning()){
			running = false;
		}

		if(!output.isRunning()){
			delay(10);
		}
	}
}

void AudioSystem::setVolume(uint8_t volume){
	output.setGain((float) volume / 255.0f);
}

bool AudioSystem::isPlaying() const{
	return running;
}
