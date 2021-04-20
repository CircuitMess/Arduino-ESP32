#include <SD.h>
#include "MixSystem.h"
#include "../../JayD.h"
#include "../Effects/LowPass.h"
#include "../Effects/HighPass.h"
#include "../Effects/Reverb.h"
#include "../Effects/BitCrusher.h"
#include "../../Settings.h"
#include "../../PerfMon.h"

MixSystem::MixSystem(const fs::File& f1, const fs::File& f2) : MixSystem(){
	open(0, f1);
	open(1, f2);
}

MixSystem::MixSystem() : audioTask("MixAudio", audioThread, 16 * 1024, this), queue(6, sizeof(MixRequest*)){
	mixer = new Mixer();

	for(int i = 0; i < 2; i++){
		effector[i] = new EffectProcessor(source[i]);

		for(int j = 0; j < 3; j++){
			effector[i]->addEffect(nullptr);
		}

		mixer->addSource(effector[i]);
	}

	i2s = new OutputI2S({
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

	i2s->setGain(0.4f*((float) Settings.get().volumeLevel) / 255.0f);
	i2s->setSource(mixer);

	fsOut = new OutputWAV();

	out = new OutputSplitter();
	out->addOutput(i2s);
}

MixSystem::~MixSystem(){
	stop();
	Sched.loop(0);
	delete out;
	delete fsOut;
	delete i2s;
	delete mixer;
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 3; j++){
			delete effector[i]->getEffect(j);
		}

		delete effector[i];
		delete speed[i];

		delete source[i];
	}
}

bool MixSystem::open(uint8_t c, const fs::File& file){
	this->file[c] = file;
	if(!file){
		Serial.println("MixSystem: file not open");
		return false;
	}

	delete source[c];
	auto source = this->source[c] = new SourceAAC(file);

	source->setRepeat(true);

	if(speed[c]){
		speed[c]->setSource(source);
	}else{
		effector[c]->setSource(source);
	}

	return true;
}

void MixSystem::audioThread(Task* task){
	MixSystem* system = static_cast<MixSystem*>(task->arg);

	Serial.println("-- MixSystem started --");

	while(task->running){
		MixRequest* request;
		while(system->queue.count()){
			system->queue.receive(&request);

			switch(request->type){
				case MixRequest::ADD_SPEED:
					system->_addSpeed(request->channel);
					break;
				case MixRequest::REMOVE_SPEED:
					system->_removeSpeed(request->channel);
					break;
				case MixRequest::SET_SPEED:
					system->_setSpeed(request->channel, request->value);
					break;
				case MixRequest::SET_EFFECT:
					system->_setEffect(request->channel, request->slot, static_cast<EffectType>(request->value));
					break;
				case MixRequest::SET_EFFECT_INTENSITY:
					system->_setEffectIntensity(request->channel, request->slot, request->value);
					break;
				case MixRequest::SET_INFO:
					system->_setInfoGenerator(request->channel, (InfoGenerator*) request->value);
					break;
				case MixRequest::SET_SEEK:
					system->_seekChannel(request->channel, (uint16_t) request->value);
					break;
				case MixRequest::RECORD:
					if(request->value == system->isRecording()) break;
					if(request->value){
						system->_startRecording();
					}else{
						system->_stopRecording();
					}
					break;
			}

			delete request;
		}

		if(system->out->isRunning()){
			Profiler.init();
			system->out->loop(0);
			Profiler.report();
		}else{
			system->running = false;
		}
	}

	system->fsOut->stop();
}

void MixSystem::start(){
	if(running) return;

	running = true;
	out->start();
	audioTask.start(1, 0);
}

void MixSystem::stop(){
	if(!running) return;

	audioTask.stop();

	while(!audioTask.isStopped()){
		Sched.loop(0);
	}

	_stopRecording();
	fileOut.close();

	out->stop();
}

bool MixSystem::isRunning(){
	return running;
}

uint16_t MixSystem::getDuration(uint8_t c){
	if(c >= 2 || !source[c]) return 0;
	return source[c]->getDuration();
}

uint16_t MixSystem::getElapsed(uint8_t c){
	if(c >= 2 || !source[c]) return 0;
	if(seekPending[c] > 0){
		return seek[c];
	}
	return source[c]->getElapsed();
}

void MixSystem::setVolume(uint8_t c, uint8_t volume){
	if(c >= 2 || !source[c]) return;
	source[c]->setVolume(volume);
}

void MixSystem::setMix(uint8_t ratio){
	if(!mixer) return;
	mixer->setMixRatio(ratio);
}

void MixSystem::addSpeed(uint8_t channel){
	if(!out->isRunning()){
		_addSpeed(channel);
		return;
	}

	if(queue.count() == queue.getQueueSize()) return;
	MixRequest* request = new MixRequest({ MixRequest::ADD_SPEED, channel });
	queue.send(&request);
}

void MixSystem::removeSpeed(uint8_t channel){
	if(!out->isRunning()){
		_removeSpeed(channel);
		return;
	}

	if(queue.count() == queue.getQueueSize()) return;
	MixRequest* request = new MixRequest({ MixRequest::REMOVE_SPEED, channel });
	queue.send(&request);
}

void MixSystem::setSpeed(uint8_t channel, uint8_t speed){
	if(!out->isRunning()){
		_setSpeed(channel, speed);
		return;
	}

	if(queue.count() == queue.getQueueSize()) return;
	MixRequest* request = new MixRequest({ MixRequest::SET_SPEED, channel, 0, speed });
	queue.send(&request);
}

void MixSystem::setEffect(uint8_t channel, uint8_t slot, EffectType type){
	if(!out->isRunning()){
		_setEffect(channel, slot, type);
		return;
	}

	if(queue.count() == queue.getQueueSize()) return;
	MixRequest* request = new MixRequest({ MixRequest::SET_EFFECT, channel, slot, static_cast<uint8_t>(type) });
	queue.send(&request);
}

void MixSystem::setEffectIntensity(uint8_t channel, uint8_t slot, uint8_t intensity){
	if(!out->isRunning()){
		setEffectIntensity(channel, slot, intensity);
		return;
	}

	if(queue.count() == queue.getQueueSize()) return;
	MixRequest* request = new MixRequest({ MixRequest::SET_EFFECT_INTENSITY, channel, slot, intensity });
	queue.send(&request);
}

void MixSystem::_addSpeed(uint8_t c){
	if(c >= 2 || !effector[c] || speed[c]) return;
	auto speed = this->speed[c] = new SpeedModifier(source[c]);
	effector[c]->setSource(speed);
}

void MixSystem::_removeSpeed(uint8_t c){
	if(c >= 2 || !effector[c] || !speed[c]) return;
	effector[c]->setSource(source[c]);
	delete speed[c];
	speed[c] = nullptr;
}

void MixSystem::_setSpeed(uint8_t c, uint8_t modifier){
	if(c >= 2 || !this->speed[c]) return;
	this->speed[c]->setModifier(modifier);
}

void MixSystem::_setEffect(uint8_t c, uint8_t s, EffectType type){
	if(c >= 2 || s >= 3 || !effector[c]) return;
	delete effector[c]->getEffect(s);
	effector[c]->setEffect(s, getEffect[type]());
}

void MixSystem::_setEffectIntensity(uint8_t c, uint8_t s, uint8_t intensity){
	if(c >= 2 || s >= 3 || !effector[c] || !effector[c]->getEffect(s)) return;
	effector[c]->getEffect(s)->setIntensity(intensity);
}

Effect* (* MixSystem::getEffect[])() = {
		[]() -> Effect*{ return nullptr; }, // None
		[]() -> Effect*{ return nullptr; }, // Speed
		[]() -> Effect*{ return new LowPass(); },
		[]() -> Effect*{ return new HighPass(); },
		[]() -> Effect*{ return new Reverb(); },
		[]() -> Effect*{ return new BitCrusher(); }
};

void MixSystem::setOutInfo(InfoGenerator* outInfoGen){
	setChannelInfo(2, outInfoGen);
}

void MixSystem::_setInfoGenerator(uint8_t channel, InfoGenerator* generator){
	if(channel > 2 || generator == nullptr) return;
	if(channel == 2){
		out->setSource(generator);
		generator->setSource(mixer);
	}else{
		mixer->setSource(channel, generator);
		generator->setSource(effector[channel]);
	}
}

void MixSystem::setChannelInfo(uint8_t channel, InfoGenerator* channelInfoGen){
	if(!out->isRunning()){
		_setInfoGenerator(channel, channelInfoGen);
		return;
	}

	if(queue.count() == queue.getQueueSize()) return;
	MixRequest* request = new MixRequest({ MixRequest::SET_INFO, channel, 0, (size_t) channelInfoGen });
	queue.send(&request);
}

void MixSystem::pauseChannel(uint8_t channel){
	mixer->pauseChannel(channel);
}

void MixSystem::resumeChannel(uint8_t channel){
	mixer->resumeChannel(channel);
	if(!out->isRunning()){
		out->start();
	}
}

void MixSystem::seekChannel(uint8_t channel, uint16_t time){
	if(!out->isRunning()){
		_seekChannel(channel, time);
		return;
	}

	seek[channel] = time;
	seekPending[channel]++;

	if(queue.count() == queue.getQueueSize()) return;
	MixRequest* request = new MixRequest({ MixRequest::SET_SEEK, channel, 0, time });
	queue.send(&request);
}

void MixSystem::_seekChannel(uint8_t channel, uint16_t time){
	if(channel > 1) return;

	if(i2s->isRunning()){
		seekPending[channel]--;
		//i2s_zero_dma_buffer((i2s_port_t) 0);
	}
	source[channel]->seek(time, SeekSet);
}

bool MixSystem::isRecording(){
	return out->getOutput(1) != nullptr;
}

void MixSystem::startRecording(){
	if(!out->isRunning()){
		_startRecording();
		return;
	}

	MixRequest* request = new MixRequest({ MixRequest::RECORD, 0, 0, 1 });
	queue.send(&request);
}

void MixSystem::stopRecording(){
	if(!out->isRunning()){
		_stopRecording();
		return;
	}

	MixRequest* request = new MixRequest({ MixRequest::RECORD, 0, 0, 0 });
	queue.send(&request);
}

void MixSystem::_startRecording(){
	if(isRecording()) return;

	if(SD.exists(recordPath)){
		SD.remove(recordPath);
	}

	fileOut = SD.open(recordPath, "w");
	if(!fileOut){
		Serial.printf("Failed opening %s for writing\n", recordPath);
		return;
	}

	fsOut->setFile(fileOut);

	out->addOutput(fsOut);

	if(out->isRunning()){
		fsOut->start();
	}
}

void MixSystem::_stopRecording(){
	if(!isRecording()) return;

	out->removeOutput(1);

	fsOut->stop();
	fileOut.close();
}

bool MixSystem::isChannelPaused(uint8_t channel){
	if(!mixer) return false;
	return mixer->isChannelPaused(channel);
}

void MixSystem::setChannelDoneCallback(uint8_t channel, void(*callback)()) {
	source[channel]->setSongDoneCallback(callback);
}
