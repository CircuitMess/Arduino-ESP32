#include <SPI.h>
#include "SDScheduler.h"

SDScheduler Sched;

SDScheduler::SDScheduler() :jobs(8, sizeof(SDJob*)){

}

void SDScheduler::addJob(SDJob *job){
	jobs.send(&job);
}

void SDScheduler::loop(uint micros) {
	if (jobs.count() == 0) {
		return;
	}

	SDJob* request = nullptr;

	while(jobs.count() > 0){
		if(!jobs.receive(&request)){
			Serial.println("Receive error");
			return;
		}

		if(request != nullptr){
			doJob(request);
		}

		delete request;
	}
}

void SDScheduler::doJob(SDJob* job){
	if(job->type == SDJob::SD_SEEK){
		bool success = job->file.seek(job->size);

		if(job->result != nullptr){
			SDResult* result = new SDResult();
			result->size = job->size * success;
			result->buffer = job->buffer;
			result->error = 0;

			*job->result = result;
		}
		return;
	}

	size_t size = job->type == SDJob::SD_READ
				  ? job->file.read(job->buffer, job->size)
				  : job->file.write(job->buffer, job->size);

	if(job->result != nullptr){
		SDResult* result = new SDResult();

		result->error = 0;
		result->buffer = job->buffer;
		result->size = size;

		*job->result = result;
	}
}

