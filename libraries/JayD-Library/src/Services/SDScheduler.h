#ifndef JAYD_LIBRARY_SDSCHEDULER_H
#define JAYD_LIBRARY_SDSCHEDULER_H

#include <FS.h>
#include <vector>
#include <Loop/LoopListener.h>
#include <Sync/Queue.h>

struct SDResult {
	uint8_t error;
	size_t size;
	uint8_t* buffer;
};

struct SDJob {
	enum { SD_WRITE, SD_READ, SD_SEEK } type;
	fs::File file;
	size_t size;
	uint8_t* buffer;
	SDResult** result;
};

class SDScheduler : public LoopListener {
public:
	SDScheduler();

	void addJob(SDJob *job);
	void loop(uint micros) override;
private:
	Queue jobs;

	void doJob(SDJob* job);

};

extern SDScheduler Sched;

#endif //JAYD_LIBRARY_SDSCHEDULER_H
