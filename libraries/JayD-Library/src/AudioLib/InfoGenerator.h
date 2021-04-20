
#ifndef JAYD_LIBRARY_INFOGENERATOR_H
#define JAYD_LIBRARY_INFOGENERATOR_H
#include "Generator.h"

class InfoGenerator : public Generator {
public:
	size_t generate(int16_t *outBuffer) override;
	int available() override;

	void setSource(Generator* gen);

protected:
	virtual void captureInfo(int16_t *outBuffer, size_t readBytes) = 0;

private:
	Generator* generator;

};


#endif //JAYD_LIBRARY_INFOGENERATOR_H
