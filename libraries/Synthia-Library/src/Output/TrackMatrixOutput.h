#ifndef SYNTHIA_LIBRARY_TRACKMATRIXOUTPUT_H
#define SYNTHIA_LIBRARY_TRACKMATRIXOUTPUT_H

#include <Devices/Matrix/MatrixPartOutput.h>

class TrackMatrixOutput : public MatrixPartOutput {
public:
	TrackMatrixOutput(MatrixOutputBuffer* output);
protected:
	std::pair<uint16_t, uint16_t> map(uint16_t x, uint16_t y) override;

};


#endif //SYNTHIA_LIBRARY_TRACKMATRIXOUTPUT_H
