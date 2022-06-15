#ifndef JAYD_LIBRARY_VUVISUALIZER_H
#define JAYD_LIBRARY_VUVISUALIZER_H

#include "Visualizer.h"
#include "../AudioLib/VuInfoGenerator.h"
#include <Loop/LoopListener.h>
#include <Devices/Matrix/Matrix.h>

class VuVisualizer : public Visualizer, public LoopListener {
public:
	VuVisualizer(Matrix *matrix);

	InfoGenerator *getInfoGenerator() override;

	void loop(uint _millis) override;

private:
	VuInfoGenerator info;

	void vu(uint16_t amp);
};


#endif //JAYD_LIBRARY_VUVISUALIZER_H
