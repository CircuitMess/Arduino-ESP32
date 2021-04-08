#include "AudioFileSourceRAM.h"

AudioFileSourceRAM::AudioFileSourceRAM()
{
	f = nullptr;
	fileSize = 0;
	pos = 0;
}
AudioFileSourceRAM::AudioFileSourceRAM(void* dataPointer, size_t size)
{
	open(dataPointer, size);
}

bool AudioFileSourceRAM::open(const char *filename)
{
	return false;
}

bool AudioFileSourceRAM::open(void* dataPointer, size_t size)
{
	if(dataPointer == nullptr || size == 0)	return false;
	f = dataPointer;
	fileSize = size;
	pos = 0;
}

AudioFileSourceRAM::~AudioFileSourceRAM()
{
	if(isOpen())
	{
		delete f;
	}
}

uint32_t AudioFileSourceRAM::read(void *data, uint32_t rdlen)
{
	uint8_t *p = (uint8_t *)data;
	if(pos + rdlen > fileSize) {
		if (pos >= fileSize) return 0;
		rdlen = fileSize - pos;
	}
	memcpy(data, f + pos, rdlen);
	pos += rdlen;
	return rdlen;
}

bool AudioFileSourceRAM::seek(int32_t _pos, int dir)
{
	if(dir == SEEK_CUR && _pos > (fileSize - 1)) return false;
	switch (dir)
	{
	case SEEK_SET:
		pos = 0;
		break;
	case SEEK_END:
		pos = fileSize - 1;
		break;
	default:
		pos = _pos;
		break;
	}
	return true;
}

bool AudioFileSourceRAM::close()
{
	f = nullptr;
	fileSize = 0;
	pos = 0;
	return true;
}

bool AudioFileSourceRAM::isOpen()
{
	if(f == nullptr || fileSize == 0)	return false;
	else return true;
}

uint32_t AudioFileSourceRAM::getSize()
{
	if (!isOpen()) return 0;
	return fileSize;
	
}
uint32_t AudioFileSourceRAM::getPos()
{
	if (!isOpen()) return 0;
	return fileSize;
}