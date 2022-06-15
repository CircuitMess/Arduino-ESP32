#ifndef JAYD_LIBRARY_PERFMON_H
#define JAYD_LIBRARY_PERFMON_H

#include <Arduino.h>
#include <vector>
#include <Sync/Mutex.h>

struct PerfMonReport {
	String name;
	uint32_t duration;
};

class PerfMon {
public:
	void init();
	void start(String name);
	void end();
	float report();

private:
	String current;
	uint32_t initTime = 0;
	uint32_t startTime = 0;

	Mutex mutex;

	std::vector<PerfMonReport> reports;
};

extern PerfMon Profiler;

#endif //JAYD_LIBRARY_PERFMON_H
