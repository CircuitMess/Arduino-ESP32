#ifndef JAYD_MIXER_H
#define JAYD_MIXER_H

#include <Arduino.h>
#include <vector>
#include "Generator.h"
#include "Source.h"

class Mixer : public Generator
{
public:
	Mixer();
	~Mixer();
	size_t generate(int16_t* outBuffer) override;
	int available() override;

	Generator* getSource(size_t index);
	void setSource(size_t index, Generator* source);

	void addSource(Generator* source);
	uint8_t getMixRatio();
	void setMixRatio(uint8_t ratio);

	void pauseChannel(uint8_t channel);
	void resumeChannel(uint8_t channel);
	bool isChannelPaused(uint8_t channel);

private:
	std::vector<Generator*> sourceList;
	std::vector<int16_t*> bufferList;
	uint8_t mixRatio = 122; //half-half by default, 0 = only first track, 255 = only second track
	std::vector<bool> pauseList;
};



#endif