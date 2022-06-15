#include "BufferedDataSource.h"

BufferedDataSource::BufferedDataSource(DataSource& ds, size_t bufSize) : ds(ds), bufSize(bufSize), readBuffer(bufSize){
	fillBuffer();
}

size_t BufferedDataSource::read(uint8_t* buf, size_t size){
	return readBuffer.read(buf, size);
}

size_t BufferedDataSource::available(){
	return readBuffer.readAvailable();
}

bool BufferedDataSource::seek(size_t pos, int mode){
	readBuffer.clear();
	if(!ds.seek(pos, mode)) return false;

	fillBuffer();
	return true;
}

void BufferedDataSource::fillBuffer(){
	size_t size = min(readBuffer.writeAvailable(), ds.available());
	auto tempBuf = (uint8_t*)malloc(size);
	ds.read(tempBuf, size);
	readBuffer.write(tempBuf, size);
	free(tempBuf);
}

size_t BufferedDataSource::size(){
	return ds.size();
}

