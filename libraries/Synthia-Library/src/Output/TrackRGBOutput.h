#ifndef SYNTHIA_LIBRARY_TRACKRGBOUTPUT_H
#define SYNTHIA_LIBRARY_TRACKRGBOUTPUT_H

#include <Devices/Matrix/MatrixPartOutput.h>

class TrackRGBOutput : public MatrixPartOutput {
public:
	TrackRGBOutput(MatrixOutputBuffer* output);
protected:
	std::pair<uint16_t, uint16_t> map(uint16_t x, uint16_t y) override;
};


#endif //SYNTHIA_LIBRARY_TRACKRGBOUTPUT_H
