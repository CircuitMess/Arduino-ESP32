#include "AudioFileSourceSerialFlash.h"

AudioFileSourceSerialFlash::AudioFileSourceSerialFlash()
{
}
AudioFileSourceSerialFlash::AudioFileSourceSerialFlash(const char *filename, uint32_t _size)
{
	open(filename);
	size = _size;
}

bool AudioFileSourceSerialFlash::open(const char *filename)
{
	while(!SerialFlash.ready());
	f = SerialFlash.open(filename);
	
	if(!f) f.close();
	return f;
}

AudioFileSourceSerialFlash::~AudioFileSourceSerialFlash()
{
	if (f) f.close();
}

uint32_t AudioFileSourceSerialFlash::read(void *data, uint32_t len)
{
	if (getPos() + len > getSize()) {
		if (getPos() >= getSize()) return 0;
		len = getSize() - getPos();
	}
	return f.read(reinterpret_cast<uint8_t*>(data), len);
}

bool AudioFileSourceSerialFlash::seek(int32_t pos, int dir)
{
	if((dir == SEEK_CUR && pos > (((int)(getSize())) - 1 - ((int)(getPos())))) || (pos > (((int)(getSize())) - 1))) return false;
	switch (dir)
	{
	case SEEK_SET:
		f.seek(pos);
		break;
	case SEEK_END:
		f.seek(getSize() - 1);
		break;
	case SEEK_CUR:
		f.seek(pos + getPos());
		break;
	}
	return true;
}

bool AudioFileSourceSerialFlash::close()
{
	f.close();
	return true;
}

bool AudioFileSourceSerialFlash::isOpen()
{
	return f;
}

uint32_t AudioFileSourceSerialFlash::getSize()
{
	if (!f) return 0;
	if (size > 0) return size;
	return f.size();
}
uint32_t AudioFileSourceSerialFlash::getPos()
{
	if (!f) return 0;
	else return f.position(); 
}