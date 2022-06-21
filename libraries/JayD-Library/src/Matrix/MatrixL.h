#ifndef JAYD_LIBRARY_MATRIXL_H
#define JAYD_LIBRARY_MATRIXL_H

#include <Devices/Matrix/MatrixPartOutput.h>

class MatrixL : public MatrixPartOutput {
public:
	MatrixL(MatrixOutputBuffer* output);
protected:
	std::pair<uint16_t, uint16_t> map(uint16_t x, uint16_t y) override;
};


#endif //JAYD_LIBRARY_MATRIXL_H
