#include "Setup.hpp"
#include "PerfMon.h"

PerfMon Profiler;
// #define PROFILER_DEBUG

void PerfMon::init(){
#ifndef PROFILER_DEBUG
	return;
#endif

	reports.clear();
	startTime = 0;
	current = "";
	initTime = micros();
}

void PerfMon::report(){
#ifndef PROFILER_DEBUG
	return;
#endif

	float total = (float) (micros() - initTime) / 1000.0f;
	float max =  1000.0f * (float) BUFFER_SAMPLES / (float) SAMPLE_RATE;
	Serial.printf("Total time: %.2f / %.2f (%.0f%%)", total, max, 100.0f * total / max);
	if(total > max){
		Serial.printf(" | %.2f over", total - max);
	}

	Serial.printf(" | H: %d B\n", ESP.getFreeHeap());

	mutex.lock();
	for(const auto& r : reports){
		Serial.printf(" -- %10s: %6.2f\n", r.name.c_str(), (float) r.duration / 1000.0f);
	}
	mutex.unlock();
	Serial.println();
}

void PerfMon::start(String name){
#ifndef PROFILER_DEBUG
	return;
#endif

	current = std::move(name);
	startTime = micros();
}

void PerfMon::end(){
#ifndef PROFILER_DEBUG
	return;
#endif

	uint32_t stopTime = micros();
	mutex.lock();
	reports.push_back({ current, stopTime - startTime });
	mutex.unlock();
}
