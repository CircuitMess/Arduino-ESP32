#include "Reverb.h"

Reverb::Reverb(){
	if(psramFound()){
		echo = static_cast<int16_t *>(ps_calloc(length, sizeof(int16_t)));
	}else{
		echo = static_cast<int16_t *>(calloc(length, sizeof(int16_t)));
	}
}

int16_t Reverb::signalProcessing(int16_t sample){

	int16_t echoSample1 = (float) echo[(length + echoCount) % (length)] * echoAmount;

	echo[echoCount++] = (float) sample;
	if(echoCount >= length){
		echoCount = 0;
	}

	float sampleScalingFactor = (0.5f * ((1.0f - ((((16.0f*(echoAmount+horizontalScale)-1)/(16.0f*(echoAmount+horizontalScale)+1))*verticalScale))) + 1.0f));
	float echoSampleScalingFactor = ((((16.0f*(echoAmount+horizontalScale)-1)/(16.0f*(echoAmount+horizontalScale)+1))*verticalScale) * 0.5f);
	//	scaling factors:	for intensity 0(min) -> sample*1 + echoSample1*0
	//						for intensity 1(max) -> sample*0.5 + echoSample1*0.5
	//	function y=(16*(x+(1)/(16))-1)/(16*(x+(1)/(16))+1)*(18/16)
	sample = (float) sample * sampleScalingFactor/(1.0f+echoSampleScalingFactor/4.0f) + (float) echoSample1 * echoSampleScalingFactor*(1.0f+echoSampleScalingFactor/4.0f);

	return (int16_t) sample;
}


void Reverb::applyEffect(int16_t *inBuffer, int16_t *outBuffer, size_t numSamples){
	for(int i = 0; i < numSamples; ++i){
		outBuffer[i] = signalProcessing(inBuffer[i]);
	}
}

void Reverb::setIntensity(uint8_t intensity){
	echoAmount = (float) intensity / 255.0f;
}

Reverb::~Reverb(){
	free(echo);
}
