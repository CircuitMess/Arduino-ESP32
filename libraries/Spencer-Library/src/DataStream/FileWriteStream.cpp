#include "FileWriteStream.h"

FileWriteStream::FileWriteStream(SerialFlashFile file) : file(file){
	buffer.reserve(bufferSize);
}

FileWriteStream::~FileWriteStream(){
	flush();
}

void FileWriteStream::write(unsigned char byte){
	buffer.push_back(byte);

	if(buffer.size() >= bufferSize){
		flush();
	}
}

void FileWriteStream::flush(){
	if(buffer.empty()) return;

	file.write(buffer.data(), buffer.size());
	buffer.resize(0);
}
