#ifndef CIRCUITMESS_AUDIO_DATASOURCE_H
#define CIRCUITMESS_AUDIO_DATASOURCE_H

#include <Arduino.h>

class DataSource {
public:
	virtual size_t read(uint8_t* buf, size_t size) = 0;
	virtual size_t available() = 0;
	virtual bool seek(size_t pos, int mode = SEEK_SET) = 0;
	virtual size_t size() = 0;
};


#endif //CIRCUITMESS_AUDIO_DATASOURCE_H
