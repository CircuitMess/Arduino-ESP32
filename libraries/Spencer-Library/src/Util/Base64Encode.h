#ifndef SPENCER_BASE64ENCODE_H
#define SPENCER_BASE64ENCODE_H


#include <queue>
#include "../DataStream/ReadStream.hpp"

class Base64Encode : public ReadStream {
public:
	explicit Base64Encode(ReadStream* source);

	bool available() override;
	unsigned char get() override;

private:
	ReadStream* source;
	std::queue<char> queue;

	void processChar();
	char getFromQueue();
};


#endif //SPENCER_BASE64ENCODE_H
