#ifndef JAYD_LIBRARY_VISUALIZER_H
#define JAYD_LIBRARY_VISUALIZER_H

#include "MatrixPartition.h"
#include "../AudioLib/InfoGenerator.h"

class Visualizer {
public:
	Visualizer(MatrixPartition *matrix);

	virtual InfoGenerator *getInfoGenerator() = 0;

protected:
	MatrixPartition *matrix;
};


#endif //JAYD_LIBRARY_VISUALIZER_H
