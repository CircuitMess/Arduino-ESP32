#ifndef JAYD_LIBRARY_MATRIXR_H
#define JAYD_LIBRARY_MATRIXR_H


#include <Devices/Matrix/MatrixPartOutput.h>

class MatrixR : public MatrixPartOutput {
public:
	MatrixR(MatrixOutputBuffer* output);

protected:
	std::pair<uint16_t, uint16_t> map(uint16_t x, uint16_t y) override;
};


#endif //JAYD_LIBRARY_MATRIXR_H
