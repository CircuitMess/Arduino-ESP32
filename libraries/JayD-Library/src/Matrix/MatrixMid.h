#ifndef JAYD_LIBRARY_MATRIXMID_H
#define JAYD_LIBRARY_MATRIXMID_H


#include "MatrixPartition.h"

class MatrixMid : public MatrixPartition {
public:
	MatrixMid(LEDmatrixImpl* matrix);

	void push() override;
	void vu(uint16_t value) override;
};


#endif //JAYD_LIBRARY_MATRIXMID_H
