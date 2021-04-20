#ifndef JAYD_LIBRARY_MATRIXL_H
#define JAYD_LIBRARY_MATRIXL_H

#include "MatrixPartition.h"

class MatrixL : public MatrixPartition {
public:
	MatrixL(LEDmatrixImpl* matrix);

	void push() override;
};


#endif //JAYD_LIBRARY_MATRIXL_H
