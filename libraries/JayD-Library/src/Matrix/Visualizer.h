#ifndef JAYD_LIBRARY_VISUALIZER_H
#define JAYD_LIBRARY_VISUALIZER_H

#include <Devices/Matrix/Matrix.h>
#include "../AudioLib/InfoGenerator.h"

class Visualizer {
public:
	Visualizer(Matrix *matrix);

	virtual InfoGenerator *getInfoGenerator() = 0;

protected:
	Matrix *matrix;
};


#endif //JAYD_LIBRARY_VISUALIZER_H
