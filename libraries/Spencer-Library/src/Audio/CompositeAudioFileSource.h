#ifndef SPENCER_COMPOSITEAUDIOFILESOURCE_H
#define SPENCER_COMPOSITEAUDIOFILESOURCE_H

#include <AudioFileSource.h>
#include <vector>

class CompositeAudioFileSource : public AudioFileSource
{
public:
	CompositeAudioFileSource();
	CompositeAudioFileSource(AudioFileSource *file);
	virtual ~CompositeAudioFileSource() override;
	
	virtual bool open(const char *filename) override;
	virtual uint32_t read(void *data, uint32_t len) override;
	virtual bool seek(int32_t pos, int dir) override;
	virtual bool close() override;
	virtual bool isOpen() override;
	virtual uint32_t getSize() override;
	virtual uint32_t getPos() override;

	AudioFileSource* getCurrentFile();

	void add(AudioFileSource* file);

private:
	std::vector<AudioFileSource*> filePointers;
	uint currentFileIndex = 0;
	size_t size = 0;
	bool _open = false;
};

#endif