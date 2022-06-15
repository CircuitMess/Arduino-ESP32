#ifndef JAYD_OUTPUTFS_H
#define JAYD_OUTPUTFS_H

#include "Output.h"
#include "../Setup.hpp"
#include <FS.h>
#include <aacenc_lib.h>
#include <Buffer/DataBuffer.h>
#include "../Data/Scheduler.h"

#define OUTFS_DECODE_BUFSIZE 1024 * NUM_CHANNELS // The output buffer size should be 6144 bits per channel excluding the LFE channel.
#define OUTFS_BUFSIZE 4 * 1024 * NUM_CHANNELS
#define OUTFS_WRITESIZE 1 * 1024 * NUM_CHANNELS // should be smaller than BUFSIZE
#define OUTFS_BUFCOUNT 1
//#define OUTFS_BUFCOUNT 16

class OutputAAC : public Output
{
public:
	OutputAAC();
	OutputAAC(const fs::File& file);
	~OutputAAC();
	void init() override;
	void deinit() override;
	const fs::File& getFile() const;
	void setFile(const fs::File& file);

protected:
	void output(size_t numBytes) override;

private:
	fs::File file;

	AACENC_BufDesc inBufDesc;
	AACENC_BufDesc outBufDesc;
	HANDLE_AACENCODER encoder;
	AACENC_InArgs inArgs;
	void setupBuffers();

	bool writePending[OUTFS_BUFCOUNT] = { false };
	SchedResult* writeResult[OUTFS_BUFCOUNT] = {nullptr };
//	void addWriteJob();
//	void processWriteJob();

	uint8_t* decodeBuffer = nullptr;
	DataBuffer* outBuffers[OUTFS_BUFCOUNT] = { nullptr };
//	std::vector<uint8_t> freeBuffers;
};


#endif