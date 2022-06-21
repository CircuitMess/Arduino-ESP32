#ifndef JAYD_LIBRARY_MATRIXBIG_H
#define JAYD_LIBRARY_MATRIXBIG_H


#include <Devices/Matrix/MatrixPartOutput.h>

class MatrixBig : public MatrixPartOutput {
public:
	MatrixBig(MatrixOutputBuffer* output);

protected:
	std::pair<uint16_t, uint16_t> map(uint16_t x, uint16_t y) override;
};


#endif //JAYD_LIBRARY_MATRIXBIG_H
