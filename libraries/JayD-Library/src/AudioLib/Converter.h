#ifndef JAYD_CONVERTER_H
#define JAYD_CONVERTER_H

#include "Generator.h"
#include "Source.h"

class Converter : public Generator {
public:
	Converter(Source* source);
	~Converter();
	size_t generate(int16_t* outBuffer) override;

private:
	Source* source;

};


#endif