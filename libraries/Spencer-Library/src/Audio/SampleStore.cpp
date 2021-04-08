#include "SampleStore.h"
#include "AudioFileSourceSerialFlash.h"
const char *prefixes[] = {"months", "numbers", "jokes", "generic", "time", "special", "weather", "weekdays", "volume", "brightness", "levels", "error", "funpack"};
AudioFileSource* SampleStore::load(SampleGroup group, const char* label)
{
	char fileName[40];
	sprintf(fileName, "%s-%s.mp3", prefixes[(int) group], label);
	return new AudioFileSourceSerialFlash(fileName);
}
