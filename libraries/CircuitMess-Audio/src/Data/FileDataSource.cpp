#include "FileDataSource.h"

FileDataSource::FileDataSource(File& file) : file(file){

}

FileDataSource::~FileDataSource(){
	file.close();
}

size_t FileDataSource::read(uint8_t* buf, size_t size){
	return file.readBytes((char*)buf, size);
}

size_t FileDataSource::available(){
	return file.available();
}

bool FileDataSource::seek(size_t pos, int mode){
	return file.seek(pos, SeekMode(mode));
}

size_t FileDataSource::size(){
	return file.size();
}

