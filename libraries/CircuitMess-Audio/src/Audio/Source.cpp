#include "Source.h"

Source::Source(DataSource& ds) : ds(ds){}

int Source::getBytesPerSample(){
	return bytesPerSample;
}

int Source::getSampleRate(){
	return sampleRate;
}

int Source::getChannels(){
	return channels;
}

void Source::setVolume(uint8_t volume){
	Source::volume = (float) volume / 255.0f;
}

uint8_t Source::getVolume() const{
	return (uint8_t)(volume * 255.0);
}

void Source::setRepeat(bool repeat){
	Source::repeat = repeat;
}
