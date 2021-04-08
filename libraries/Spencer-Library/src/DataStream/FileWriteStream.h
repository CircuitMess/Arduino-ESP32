#ifndef SPENCER_FILEWRITESTREAM_H
#define SPENCER_FILEWRITESTREAM_H


#include "WriteStream.hpp"
#include <SerialFlash.h>
#include <vector>

class FileWriteStream : public WriteStream {
public:
	explicit FileWriteStream(SerialFlashFile file);
	virtual ~FileWriteStream();

	void write(unsigned char byte) override;
	void flush();

private:
	const uint32_t bufferSize = 256;

	SerialFlashFile file;
	std::vector<unsigned char> buffer;

};


#endif //SPENCER_FILEWRITESTREAM_H
