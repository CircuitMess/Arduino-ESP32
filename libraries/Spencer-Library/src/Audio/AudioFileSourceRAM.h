#ifndef SPENCER_AUDIOFILESOURCERAM_H
#define SPENCER_AUDIOFILESOURCERAM_H

#include <AudioFileSource.h>
#include <SerialFlash.h>


class AudioFileSourceRAM : public AudioFileSource
{
public:
	AudioFileSourceRAM();
	AudioFileSourceRAM(void* dataPointer, size_t size);
	virtual ~AudioFileSourceRAM() override;
	
	virtual bool open(const char *filename) override;
	bool open(void* dataPointer, size_t size);
	virtual uint32_t read(void *data, uint32_t len) override;
	virtual bool seek(int32_t pos, int dir) override;
	virtual bool close() override;
	virtual bool isOpen() override;
	virtual uint32_t getSize() override;
	virtual uint32_t getPos() override;
private:
	void* f;
	size_t fileSize;
	uint32_t pos;
};


#endif