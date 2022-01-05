#ifndef JAYD_LIBRARY_VUINFOGENERATOR_H
#define JAYD_LIBRARY_VUINFOGENERATOR_H

#include "InfoGenerator.h"

class VuInfoGenerator : public InfoGenerator{
public:
	uint16_t getVu();

protected:
	void captureInfo(int16_t *outBuffer, size_t readSamples) override;

private:
	uint16_t vu = 0;

};


#endif //JAYD_LIBRARY_VUINFOGENERATOR_H
