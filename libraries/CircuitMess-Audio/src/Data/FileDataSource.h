#ifndef CIRCUITMESS_AUDIO_FILEDATASOURCE_H
#define CIRCUITMESS_AUDIO_FILEDATASOURCE_H

#include "DataSource.h"
#include <FS.h>

class FileDataSource : public DataSource {
public:
	FileDataSource(File& file);
	virtual ~FileDataSource();
	size_t read(uint8_t* buf, size_t size) override;
	size_t available() override;
	bool seek(size_t pos, int mode) override;
	size_t size() override;

private:
	File file;
};


#endif //CIRCUITMESS_AUDIO_FILEDATASOURCE_H
