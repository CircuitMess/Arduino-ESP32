#include "Scheduler.h"

Scheduler::Scheduler() : jobs(8, sizeof(SchedJob*)){

}

void Scheduler::addJob(SchedJob *job){
	jobs.send(&job);
}

void Scheduler::loop(uint micros) {
	if (jobs.count() == 0) {
		return;
	}

	SchedJob* request = nullptr;

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

void Scheduler::doJob(SchedJob* job){
	beforeJob();

	if(job->type == SchedJob::SEEK){
		bool success = job->ds.seek(job->size);

		if(job->result != nullptr){
			SchedResult* result = new SchedResult();
			result->size = job->size * success;
			result->buffer = job->buffer;
			result->error = 0;

			*job->result = result;
		}
		afterJob();
		return;
	}

/*	size_t size = job->type == SchedJob::READ
				  ? job->ds.read(job->buffer, job->size)
				  : job->ds.write(job->buffer, job->size);*/

	size_t size = job->ds.read(job->buffer, job->size);

	if(job->result != nullptr){
		SchedResult* result = new SchedResult();

		result->error = 0;
		result->buffer = job->buffer;
		result->size = size;

		*job->result = result;
	}
	afterJob();
}

