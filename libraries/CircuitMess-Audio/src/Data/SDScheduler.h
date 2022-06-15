#ifndef JAYD_LIBRARY_SDSCHEDULER_H
#define JAYD_LIBRARY_SDSCHEDULER_H

#include <FS.h>
#include <vector>
#include <Loop/LoopListener.h>
#include <Sync/Queue.h>
#include "Scheduler.h"

class SDScheduler : public Scheduler {
public:
	void beforeJob() override;
	void afterJob() override;
};

extern SDScheduler Sched;

#endif //JAYD_LIBRARY_SDSCHEDULER_H
