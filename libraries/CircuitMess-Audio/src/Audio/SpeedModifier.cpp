#include "SpeedModifier.h"
#include "../Setup.hpp"

SpeedModifier::SpeedModifier(Generator* source) : source(source){
	dataBuffer = new DataBuffer(BUFFER_SIZE * 3, true);
}

SpeedModifier::~SpeedModifier() noexcept{
	delete dataBuffer;
}

size_t SpeedModifier::generate(int16_t *outBuffer){
	if(dataBuffer->readAvailable() < (float) BUFFER_SIZE * 2){
		fillBuffer();
	}

	float sourcePtr = remainder;
	size_t destinationPtr = 0;

	while(destinationPtr < BUFFER_SAMPLES && floor(sourcePtr) < dataBuffer->readAvailable() / 2){
		outBuffer[destinationPtr++] = reinterpret_cast<const int16_t*>(dataBuffer->readData())[(int) floor(sourcePtr)];
		sourcePtr += speed;
	}

	remainder = sourcePtr - floor(sourcePtr);
	dataBuffer->readMove(floor(sourcePtr) * BYTES_PER_SAMPLE * NUM_CHANNELS);

	return destinationPtr;
}

int SpeedModifier::available(){
	if(source == nullptr) return 0;
	return source->available() + dataBuffer->readAvailable() / (BYTES_PER_SAMPLE * NUM_CHANNELS);
}

void SpeedModifier::setModifier(uint8_t modifier){
	speed = (float) modifier * (1.0f / 255.0f) + 0.5f;
}

void SpeedModifier::setSpeed(float speed){
	this->speed = speed;
}

void SpeedModifier::fillBuffer(){
	while(dataBuffer->readAvailable() < BUFFER_SIZE * 2){
		size_t generated = source->generate(reinterpret_cast<int16_t*>(dataBuffer->writeData()));
		if(!generated) break;
		dataBuffer->writeMove(generated * BYTES_PER_SAMPLE * NUM_CHANNELS);
	}
}

void SpeedModifier::setSource(Generator* source){
	SpeedModifier::source = source;
}

void SpeedModifier::resetBuffers(){
	dataBuffer->clear();
}
