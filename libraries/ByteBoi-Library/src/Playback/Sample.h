#ifndef BYTEBOI_LIBRARY_SAMPLE_H
#define BYTEBOI_LIBRARY_SAMPLE_H

#include <CMAudio.h>
#include <Audio/Source.h>
#include "PlaybackSystem.h"

class Sample {
friend PlaybackSystem;
public:
	Sample(fs::File file, bool preload = false);
	virtual ~Sample();

	bool isLooping() const;
	void setLooping(bool looping);

private:
	Source* source = nullptr;
	fs::File sourceFile;

	Source* getSource() const;

	bool looping = false;
};


#endif //BYTEBOI_LIBRARY_SAMPLE_H
