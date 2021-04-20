#include "PlaybackSystem.h"
#include "../../JayD.h"
#include "../../Settings.h"

PlaybackSystem::PlaybackSystem(const fs::File& f) : PlaybackSystem(){
	open(f);
}

PlaybackSystem::PlaybackSystem() : audioTask("MixAudio", audioThread, 4 * 1024, this), queue(4, sizeof(PlaybackRequest*)){
	out = new OutputI2S({
								.mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_TX),
								.sample_rate = SAMPLE_RATE,
								.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
								.channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
								.communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
								.intr_alloc_flags = 0,
								.dma_buf_count = 16,
								.dma_buf_len = 512,
								.use_apll = false
						}, i2s_pin_config, I2S_NUM_0);

	out->setGain(0.4f*((float) Settings.get().volumeLevel) / 255.0f);
}

PlaybackSystem::~PlaybackSystem(){
	stop();
	Sched.loop(0);
	delete out;
	delete source;
}

bool PlaybackSystem::open(const fs::File& file){
	this->file = file;
	if(!file){
		Serial.println("PlaybackSystem: file not open");
		return false;
	}

	delete source;
	this->source = new SourceAAC(file);

	out->setSource(this->source);

	return true;
}

void PlaybackSystem::audioThread(Task* task){
	PlaybackSystem* system = static_cast<PlaybackSystem*>(task->arg);

	Serial.println("-- PlaybackSystem started --");

	while(task->running){
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

		if(system->out->isRunning()){
			system->out->loop(0);
		}
	}
}

void PlaybackSystem::start(){
	if(running) return;

	running = true;
	out->start();
	audioTask.start(1, 0);
}

void PlaybackSystem::stop(){
	if(!running) return;

	audioTask.stop(true);
	out->stop();
	running = false;
}

bool PlaybackSystem::isRunning(){
	return running;
}

uint16_t PlaybackSystem::getDuration(){
	if(!source) return 0;
	return source->getDuration();
}

uint16_t PlaybackSystem::getElapsed(){
	if(!source) return 0;
	return source->getElapsed();
}

void PlaybackSystem::setVolume(uint8_t volume){
	if(!source) return;
	source->setVolume(volume);
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

void PlaybackSystem::setRepeat(bool _repeat) {
	source->setRepeat(_repeat);
}

void PlaybackSystem::_seek(uint16_t time) {
	if(out->isRunning()) {
		i2s_zero_dma_buffer((i2s_port_t) 0);
	}
	source->seek(time, SeekSet);
}

void PlaybackSystem::updateGain(){
	out->setGain(0.4f*((float) Settings.get().volumeLevel) / 255.0f);
}
