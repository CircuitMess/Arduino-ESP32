#ifndef JAYD_LIBRARY_VUVISUALIZER_H
#define JAYD_LIBRARY_VUVISUALIZER_H

#include "Visualizer.h"
#include "../AudioLib/VuInfoGenerator.h"

class VuVisualizer : public Visualizer, public LoopListener {
public:
	VuVisualizer(MatrixPartition *matrix);

	InfoGenerator *getInfoGenerator() override;

	void loop(uint _millis) override;

private:
	VuInfoGenerator info;
};


#endif //JAYD_LIBRARY_VUVISUALIZER_H
