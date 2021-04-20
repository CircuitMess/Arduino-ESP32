#ifndef JAYD_LIBRARY_MATRIXR_H
#define JAYD_LIBRARY_MATRIXR_H


#include "MatrixPartition.h"

class MatrixR : public MatrixPartition {
public:
	MatrixR(LEDmatrixImpl* matrix);

	void push() override;
};


#endif //JAYD_LIBRARY_MATRIXR_H
