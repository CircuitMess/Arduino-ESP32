#ifndef SPENCER_BASE64DECODE_H
#define SPENCER_BASE64DECODE_H


#include <queue>
#include "../DataStream/WriteStream.hpp"

class Base64Decode : public WriteStream {
public:
	explicit Base64Decode(WriteStream* destination);

	uint8_t write_return(unsigned char byte);
	void write(unsigned char byte) override;

private:
	WriteStream* destination;
	std::vector<unsigned char> buffer;

	uint8_t writeBuffer();
	char getFromQueue();
};


#endif //SPENCER_BASE64DECODE_H
