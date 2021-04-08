#include "Playback.h"
#include "Compression.h"
PlaybackImpl Playback;
PlaybackImpl::PlaybackImpl()
{
	wav = new AudioGeneratorWAV();
	mp3 = new AudioGeneratorMP3();
	i2s = new I2S();
}

PlaybackImpl::~PlaybackImpl()
{
	delete file;
}
void PlaybackImpl::begin(I2S* i2s)
{
	this->i2s = i2s;
	out = new AudioOutputI2S(0,0,16,0);
	out->SetRate(16000);
	out->SetPinout(16, 27, 4);
	out->SetChannels(1);
	out->SetOutputModeMono(1);
	out->SetGain(volume);
}
void PlaybackImpl::loop(uint _time)
{
	if(wav != nullptr && wav->isRunning()){
		if(!wav->loop()){
			stopPlayback(true);
		}
	}
	if(mp3 != nullptr && mp3->isRunning()){
		if(!mp3->loop()){
			stopPlayback(true);
		}
	}
}
void PlaybackImpl::playWAV(AudioFileSource* _file)
{
	if(_file == nullptr) return;
	stopPlayback(true);

	i2s->begin();
	delay(50);
	file = _file;
	wav->begin(file, out);
}
void PlaybackImpl::playWAV(const char* path)
{
	if(path == nullptr) return;
	stopPlayback(true);

	i2s->begin();
	delay(50);
	file = new AudioFileSourceSerialFlash(path);
	wav->begin(file, out);
}
void PlaybackImpl::playMP3(AudioFileSource* _file)
{
	if(_file == nullptr) return;
	stopPlayback(true);

	i2s->begin();
	delay(50);
	file = _file;
	if(!mp3->begin(file, out))
	{
		return;
	}
}
void PlaybackImpl::playMP3(const char* path)
{
	if(path == nullptr) return;
	stopPlayback(true);

	i2s->begin();
	delay(50);
	file = new AudioFileSourceSerialFlash(path);
	mp3->begin(file, out);
}
void PlaybackImpl::stopPlayback(bool executeCallback)
{
	if(wav != nullptr && wav->isRunning()){
		wav->stop();
	}
	if(mp3 != nullptr && mp3->isRunning()){
		mp3->stop();
	}

	i2s->stop();
	delete file;
	file = nullptr;

	if(playbackDoneCallback != nullptr && executeCallback){
		void (* callback)() = playbackDoneCallback;
		playbackDoneCallback = nullptr;
		callback();
	}else if(playbackDoneCallback != nullptr){
		playbackDoneCallback = nullptr;
	}
}
bool PlaybackImpl::isRunning()
{
	return i2s->isInited();
}
void PlaybackImpl::setPlaybackDoneCallback(void (*callback)())
{
	if(callback == nullptr || !isRunning()) return;
	playbackDoneCallback = callback;
}
void PlaybackImpl::setVolume(float vol)
{
	volume = vol;
	out->SetGain(volume);
}
float PlaybackImpl::getVolume()
{
	return volume;
}