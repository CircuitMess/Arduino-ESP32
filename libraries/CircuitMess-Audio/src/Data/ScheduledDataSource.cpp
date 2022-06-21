#include "ScheduledDataSource.h"
#include "../PerfMon.h"

ScheduledDataSource::ScheduledDataSource(Scheduler& sched, DataSource& ds) : sched(sched), ds(ds), readBuffer(AAC_READ_BUFFER){
	addReadJob(true);
}

size_t ScheduledDataSource::read(uint8_t* buf, size_t size){
	processReadJob();
	size_t res = readBuffer.read(buf, size);
	addReadJob();
	return res;
}

size_t ScheduledDataSource::available(){
	return ds.available();
}

bool ScheduledDataSource::seek(size_t pos, int mode){
	if(readJobPending){
		while(readResult == nullptr){
			delayMicroseconds(1);
		}

		free(readResult->buffer);
		delete readResult;
		readResult = nullptr;
		readJobPending = false;
	}

	sched.addJob(new SchedJob {
			.type = SchedJob::SEEK,
			.ds = ds,
			.size = pos,
			.buffer = nullptr,
			.result = nullptr
	});
	addReadJob();
}

size_t ScheduledDataSource::size(){
	return ds.size();
}


void ScheduledDataSource::addReadJob(bool full){
	if(readJobPending) return;

	delete readResult;
	readResult = nullptr;

	size_t size = full ? readBuffer.writeAvailable() : readChunkSize;


	if(size == 0 || readBuffer.writeAvailable() < size){
		return;
	}

	uint8_t* buf;
	if(size <= readChunkSize || !psramFound()){
		buf = static_cast<uint8_t*>(malloc(size));
	}else{
		buf = static_cast<uint8_t*>(ps_malloc(size));
	}

	sched.addJob(new SchedJob {
			.type = SchedJob::READ,
			.ds = ds,
			.size = size,
			.buffer = buf,
			.result = &readResult
	});
	readJobPending = true;
}

void ScheduledDataSource::processReadJob(){
	if(readResult == nullptr){
		if(readBuffer.readAvailable() < BUFFER_SIZE){
			while(readResult == nullptr){ //waiting for other thread
				delayMicroseconds(1);
			}
			Profiler.end();
		}else{
			return;
		}
	}

/*	if(readBuffer.readAvailable() < BUFFER_SIZE){
		// chunk size is smaller than BUFFER_SIZE
		// lets recurse this shit

		addReadJob();
		processReadJob();
		return;
	}*/


	readBuffer.write(readResult->buffer, readResult->size);
	free(readResult->buffer);

	delete readResult;
	readResult = nullptr;

	readJobPending = false;
}