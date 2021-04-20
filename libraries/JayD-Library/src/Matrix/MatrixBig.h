#ifndef JAYD_LIBRARY_MATRIXBIG_H
#define JAYD_LIBRARY_MATRIXBIG_H


#include "MatrixPartition.h"

class MatrixBig : public MatrixPartition {
public:
	MatrixBig(LEDmatrixImpl* matrix);

	void push() override;
};


#endif //JAYD_LIBRARY_MATRIXBIG_H
