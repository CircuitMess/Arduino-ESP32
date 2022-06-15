#ifndef JAYD_OUTPUTWAV_H
#define JAYD_OUTPUTWAV_H

#include "Output.h"
#include "../Setup.hpp"
#include <FS.h>
#include <aacenc_lib.h>
#include <Buffer/DataBuffer.h>
#include "../Data/Scheduler.h"

#define OUTWAV_BUFSIZE 2 * 1024 * NUM_CHANNELS
#define OUTWAV_WRITESIZE 1 * 1024 * NUM_CHANNELS // should be smaller than BUFSIZE
#define OUTWAV_BUFCOUNT 16

class OutputWAV : public Output
{
public:
	OutputWAV();
	OutputWAV(const fs::File& file);
	~OutputWAV();
	void init() override;
	void deinit() override;
	const fs::File& getFile() const;
	void setFile(const fs::File& file);

protected:
	void output(size_t numBytes) override;

private:
	const char* path;
	fs::File file;
	size_t dataLength;

	void writeHeaderWAV(size_t size);

/*	bool writePending[OUTWAV_BUFCOUNT] = { false };
	SchedResult* writeResult[OUTWAV_BUFCOUNT] = {nullptr };
	void addWriteJob();
	void processWriteJob();

	DataBuffer* outBuffers[OUTWAV_BUFCOUNT] = { nullptr };
	std::vector<uint8_t> freeBuffers;*/
};


#endif