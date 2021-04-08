#include <string>
#include "Base64Encode.h"

static const std::string base64table = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/.";

Base64Encode::Base64Encode(ReadStream* source) : source(source){ }

bool Base64Encode::available(){
	return !queue.empty() || source->available();
}

unsigned char Base64Encode::get(){
	if(!available()) return 0;

	if(!queue.empty()){
		return getFromQueue();
	}

	processChar();
	return getFromQueue();
}

void Base64Encode::processChar(){
	auto putChar = [&](uint i){
		char c = (i == -1 ? '=' : base64table[i]);
		queue.push(c);
	};

	/* ## Fetch bytes # */

	int noBytes = 0;
	char bytes[3];
	while(source->available() && noBytes < 3){
		bytes[noBytes++] = source->get();
	}

	/* ## Convert ## */

	putChar((bytes[0] & 0xfc) >> 2);

	if(noBytes == 1){
		putChar((bytes[0] & 0x03) << 4);
		putChar(-1);
		putChar(-1);
		return;
	}

	putChar(((bytes[0] & 0x03) << 4) + ((bytes[1] & 0xf0) >> 4));

	if(noBytes == 2){
		putChar((bytes[1] & 0x0f) << 2);
		putChar(-1);
		return;
	}

	putChar(((bytes[1] & 0x0f) << 2) + ((bytes[2] & 0xc0) >> 6));
	putChar(bytes[2] & 0x3f);
}

char Base64Encode::getFromQueue(){
	char val = queue.front();
	queue.pop();
	return val;
}
