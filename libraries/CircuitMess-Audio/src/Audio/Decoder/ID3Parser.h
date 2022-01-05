#ifndef JAYD_ID3PARSER_H
#define JAYD_ID3PARSER_H

#include <Arduino.h>
#include <FS.h>

struct ID3Metadata {
	ID3Metadata(){}

	size_t headerSize = 0;
	char* title = nullptr;
	char* album = nullptr;
	char* artist = nullptr;
};

class ID3Parser {
public:
	ID3Parser(fs::File& file);

	ID3Metadata parse();

private:
	fs::File file;

	bool unsync;
	bool extendedHeader;
	bool experimental;

	uint8_t* data;
	size_t dataPtr = 0;
	size_t size; // without header

	template<typename T>
	static void swendian(T* data){
		size_t swapped = 0;

		for(int i = 0; i < sizeof(T); i++){
			swapped = (swapped << 8) | ((uint8_t*) data)[i];
		}

		memcpy(data, &swapped, sizeof(T));
	}

	template<typename T>
	static void sync(T* data){
		uint32_t mask = 0x7F;
		uint32_t value = *data & mask;
		mask = mask << 8;

		for(int i = 1; i < sizeof(T); i++){
			value |= (value & mask) >> i;
		}

		*data = value;
	}
};


#endif //JAYD_ID3PARSER_H
