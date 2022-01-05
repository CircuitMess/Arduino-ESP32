#include "OutputSplitter.h"
#include "../Setup.hpp"

OutputSplitter::OutputSplitter(){

}

OutputSplitter::~OutputSplitter(){

}

void OutputSplitter::output(size_t numSamples){
	for(auto output : outputs){
		if(output == nullptr) continue;
		memcpy(output->inBuffer, inBuffer, BUFFER_SIZE);

		if(output->gain != 1.0f){
			for(uint32_t i = 0; i < numSamples * NUM_CHANNELS; i++){
				output->inBuffer[i] = (float) output->inBuffer[i] * output->gain;
			}
		}

		output->output(numSamples);
	}
}

void OutputSplitter::init(){
	for(auto output : outputs){
		if(output == nullptr) continue;
		output->start();
	}
}

void OutputSplitter::deinit(){
	for(auto output : outputs){
		if(output == nullptr) continue;
		output->stop();
	}
}

void OutputSplitter::checkTimed(){
	timed = false;
	for(const auto& output : outputs){
		if(output == nullptr) continue;
		timed |= output->timed;
	}
}

void OutputSplitter::setOutput(size_t i, Output *output){
	if(i >= outputs.size()) return;
	outputs[i] = output;
	checkTimed();
}

void OutputSplitter::addOutput(Output *output){
	outputs.push_back(output);
	checkTimed();
}

Output* OutputSplitter::getOutput(size_t i){
	if(i >= outputs.size()) return nullptr;
	return outputs[i];
}

void OutputSplitter::removeOutput(size_t i){
	if(i >= outputs.size()) return;
	outputs.erase(outputs.begin() + i);
	checkTimed();
}
