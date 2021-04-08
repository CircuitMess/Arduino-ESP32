#include <string>
#include "Base64Decode.h"

static const std::string base64table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/.";

Base64Decode::Base64Decode(WriteStream* destination) : destination(destination){
	buffer.reserve(4);
}

uint8_t Base64Decode::write_return(unsigned char byte){
	buffer.push_back(byte);
	uint8_t written = 0;
	if(buffer.size() == 4){
		written = writeBuffer();
		buffer.clear();
	}
	return written;
}
void Base64Decode::write(unsigned char byte){
	write_return(byte);
}

uint8_t Base64Decode::writeBuffer(){
	uint8_t written = 0;
	uint i1 = base64table.find(buffer[0]);
	uint i2 = base64table.find(buffer[1]);
	destination->write((i1 << 2) + ((i2 & 0x30) >> 4));
	written++;

	if(buffer[2] == '=') return written;
	uint i3 = base64table.find(buffer[2]);
	destination->write(((i2 & 0x0f) << 4) + ((i3 & 0x3c) >> 2));
	written++;

	if(buffer[3] == '=') return written;
	uint i4 = base64table.find(buffer[3]);
	destination->write(((i3 & 0x03) << 6) + i4);
	written++;

	return written;
}
