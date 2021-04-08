#ifndef SPENCER_FILEREADSTREAM_H
#define SPENCER_FILEREADSTREAM_H


#include "ReadStream.hpp"
#include "../Audio/AudioFileSourceSerialFlash.h"
#include <SerialFlash.h>
#include <vector>

class FileReadStream : public ReadStream {
public:
	explicit FileReadStream(SerialFlashFile* source);

	bool available() override;
	unsigned char get() override;

	void clearBuffer();

private:
	const uint32_t bufferSize = 256;

	SerialFlashFile* source;
	std::vector<unsigned char> buffer;

};


#endif //SPENCER_FILEREADSTREAM_H
