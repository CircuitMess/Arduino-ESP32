#ifndef JAYD_LIBRARY_ROUNDVUVISUALISER_H
#define JAYD_LIBRARY_ROUNDVUVISUALISER_H

#include "Visualizer.h"
#include "../AudioLib/VuInfoGenerator.h"
#include <Loop/LoopListener.h>

class RoundVuVisualiser: public Visualizer, public LoopListener{

public:
	RoundVuVisualiser(Matrix *matrix);

	InfoGenerator *getInfoGenerator() override;

	void loop(uint _millis) override;

private:
	VuInfoGenerator info;
	void drawSquare(uint8_t width, uint8_t value);
};


#endif //JAYD_LIBRARY_ROUNDVUVISUALISER_H
