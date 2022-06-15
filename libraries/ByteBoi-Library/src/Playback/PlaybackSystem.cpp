#include "PlaybackSystem.h"
#include "../Settings.h"
#include "../Pins.hpp"
#include "Sample.h"


PlaybackSystem Playback;

PlaybackSystem::PlaybackSystem(Sample* sample) : PlaybackSystem(){
	open(sample);
}

PlaybackSystem::PlaybackSystem() : audioTask("MixAudio", audioThread, 4 * 1024, this), queue(4, sizeof(PlaybackRequest*)){
	oscillator = new Oscillator();
	mixer = new Mixer();

	mixer->addSource(oscillator);
	mixer->addSource(nullptr);
	mixer->setMixRatio(127);
}

PlaybackSystem::~PlaybackSystem(){
	stop();
	delete out;
	delete oscillator;
	delete mixer;
}

void PlaybackSystem::begin(){
	if(out){
		delete out;
	}

	out = new OutputDAC(SPEAKER_PIN, SPEAKER_SD);
	out->setSource(mixer);
	updateGain();
}

void PlaybackSystem::tone(uint16_t freq, uint16_t duration, Wave::Type type){
	if(Settings.get().volume == 0) return;

	oscillator->tone(freq, duration, type);
	mixer->resumeChannel(0);
	start();
}

void PlaybackSystem::noTone(){
	oscillator->noTone();
}

void PlaybackSystem::audioThread(Task* task){
	PlaybackSystem* system = static_cast<PlaybackSystem*>(task->arg);

	while(task->running){
		vTaskDelay(1);

		PlaybackRequest* request;
		while(system->queue.count()){
			system->queue.receive(&request);

			switch(request->type){
				case PlaybackRequest::SEEK:
					system->_seek(request->value);
					break;

			}

			delete request;
		}

		if(!task->running) break;

		system->Sched.loop(0);

		system->setMixRatio();

		if(system->out->isRunning()){
			system->out->loop(0);
		}else{
			break;
		}

		Sample* sample = system->currentSample;
		if(sample != nullptr && sample->getSource()->available() == 0){
			if(sample->isLooping()){
				sample->getSource()->seek(0, fs::SeekSet);
				system->out->start();
			}else{
				system->mixer->setSource(1, nullptr);
				system->currentSample = nullptr;
			}
		}
	}

	system->running = false;
}

bool PlaybackSystem::open(Sample* sample){
	this->currentSample = sample;
	return true;
}

void PlaybackSystem::play(Sample* sample){
	if(Settings.get().volume == 0) return;
	if(sample->getSource() == nullptr) return;
	open(sample);
	start();
}

void PlaybackSystem::start(){
	if(Settings.get().volume == 0) return;

	if(currentSample != nullptr){
		mixer->setSource(1, currentSample->getSource());
		mixer->resumeChannel(1);
	}

	setMixRatio();

	if(running) return;

	running = true;
	out->start();
	audioTask.start(1, 0);
}

void PlaybackSystem::stop(){
	mixer->setSource(1, nullptr);
	currentSample = nullptr;
	setMixRatio();
}

void PlaybackSystem::setMixRatio(){
	size_t osc = oscillator->available();
	size_t sam = currentSample ? currentSample->getSource()->available() : 0;

	if(osc && sam){
		mixer->setMixRatio(50);
	}else if(osc){
		mixer->setMixRatio(0);
	}else if(sam){
		mixer->setMixRatio(255);
	}
}

bool PlaybackSystem::isRunning(){
	return running;
}

uint16_t PlaybackSystem::getDuration(){
	if(!currentSample) return 0;
	return currentSample->getSource()->getDuration();
}

uint16_t PlaybackSystem::getElapsed(){
	if(!currentSample) return 0;
	return currentSample->getSource()->getElapsed();
}

void PlaybackSystem::seek(uint16_t time) {
	if (!out->isRunning()) {
		_seek(time);
		return;
	}

	if (queue.count() == queue.getQueueSize()) return;
	PlaybackRequest *request = new PlaybackRequest({PlaybackRequest::SEEK, time});
	queue.send(&request);
}

void PlaybackSystem::_seek(uint16_t time) {
	if(!currentSample) return;

	currentSample->getSource()->seek(time, SeekSet);
}

void PlaybackSystem::updateGain(){
	if(!out) return;

	out->setGain((float) Settings.get().volume / 255.0f);
}

SDScheduler& PlaybackSystem::getSched(){
	return Sched;
}
