#ifndef CIRCUITMESS_AUDIO_BUFFEREDDATASOURCE_H
#define CIRCUITMESS_AUDIO_BUFFEREDDATASOURCE_H

#include "DataSource.h"
#include <Buffer/RingBuffer.h>

class BufferedDataSource : public DataSource {
public:
	BufferedDataSource(DataSource& ds, size_t bufSize);
	size_t read(uint8_t* buf, size_t size) override;
	size_t available() override;
	bool seek(size_t pos, int mode) override;
	size_t size() override;
private:
	DataSource& ds;
	size_t bufSize = 0;
	RingBuffer readBuffer;

	void fillBuffer();
};


#endif //CIRCUITMESS_AUDIO_BUFFEREDDATASOURCE_H
