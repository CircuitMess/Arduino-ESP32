
#include "InfoGenerator.h"

size_t InfoGenerator::generate(int16_t *outBuffer) {
	size_t returned = generator->generate(outBuffer);
	captureInfo(outBuffer, returned);
	return returned;
}

void InfoGenerator::setSource(Generator *gen) {
	generator = gen;
}

int InfoGenerator::available() {
	return generator->available();
}

