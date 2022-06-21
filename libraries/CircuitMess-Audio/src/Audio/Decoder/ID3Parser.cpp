#include "ID3Parser.h"

#define FLAG_UNSYNC 0b10000000
#define FLAG_EHEAD 0b01000000
#define FLAG_EXPER 0b00100000

#define FRAMEFLAG_COMP 0b10000000
#define FRAMEFLAG_ENC 0b01000000

struct ID3Header {
	char id[3];
	uint8_t version;
	uint8_t revision;
	uint8_t flags;
	uint32_t size;
} __attribute__((packed));

ID3Parser::ID3Parser(DataSource &ds) : ds(ds){ }

ID3Metadata ID3Parser::parse(){
	ID3Header header;

	ds.seek(0);
	ds.read(reinterpret_cast<uint8_t*>(&header), sizeof(ID3Header));
	swendian(&header.size);
	sync(&header.size);

	size = header.size;
	unsync = header.flags & FLAG_UNSYNC;
	extendedHeader = header.flags & FLAG_EHEAD;
	experimental = header.flags & FLAG_EXPER;

	data = static_cast<uint8_t*>(malloc(size));
	ds.read(data, size);
	dataPtr = 0;

	if(extendedHeader){
		uint32_t ehSize = *reinterpret_cast<uint32_t*>(data);
		swendian(&ehSize);
		sync(&ehSize);

		dataPtr += ehSize;
	}

	ID3Metadata id3;
	id3.headerSize = size + 10;

	while(dataPtr < size){
		if(data[dataPtr] != 'T'){
			printf("Error parsing ID3 header on byte %ld\n", dataPtr);
			free(data);
			return {};
		}

		char frameID[3];
		memcpy(frameID, data + dataPtr + 1, 3);
		dataPtr += 4;

		uint32_t frameSize = *reinterpret_cast<uint32_t*>(data + dataPtr);
		dataPtr += 4;

		uint8_t frameTags1 = *reinterpret_cast<uint8_t*>(data + dataPtr);
		uint8_t frameTags2 = *reinterpret_cast<uint8_t*>(data + dataPtr + 1);
		dataPtr += 2;

		swendian(&frameSize);
		if(unsync){
			sync(&frameSize);
		}

		if((frameTags2 & FRAMEFLAG_COMP) || (frameTags2 & FRAMEFLAG_ENC)){
			dataPtr += frameSize;
			continue;
		}

		char** field;
		if(memcmp(frameID, "IT2", 3) == 0){
			field = &id3.title;
		}else if(memcmp(frameID, "ALB", 3) == 0){
			field = &id3.album;
		}else if(memcmp(frameID, "PE1", 3) == 0){
			field = &id3.artist;
		}else{
			dataPtr += frameSize;
			continue;
		}

		*field = static_cast<char*>(calloc(frameSize + 1, 1));
		memcpy(*field, data + dataPtr, frameSize);
		dataPtr += frameSize;

		uint32_t dataStart;
		for(dataStart = 0; dataStart < frameSize && (*field)[dataStart] == 0; dataStart++);
		memcpy(*field, *field + dataStart, frameSize - dataStart);
		(*field)[frameSize - dataStart] = 0;
	}

	free(data);
	return id3;
}
