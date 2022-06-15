#ifndef CIRCUITMESS_AUDIO_SCHEDULEDDATASOURCE_H
#define CIRCUITMESS_AUDIO_SCHEDULEDDATASOURCE_H

#include <Buffer/RingBuffer.h>
#include "DataSource.h"
#include "../Setup.hpp"
#include "Scheduler.h"

class ScheduledDataSource : public DataSource {
public:
	ScheduledDataSource(Scheduler& sched, DataSource& ds);
	size_t read(uint8_t* buf, size_t size) override;
	size_t available() override;
	bool seek(size_t pos, int mode) override;
	size_t size() override;

private:
	DataSource& ds;
	Scheduler& sched;

	RingBuffer readBuffer;
	bool readJobPending = false;
	static const size_t readChunkSize = BUFFER_SIZE * 4;

	SchedResult* readResult = nullptr;
	void addReadJob(bool full = false);
	void processReadJob();
};


#endif //CIRCUITMESS_AUDIO_SCHEDULEDDATASOURCE_H
