#ifndef JAYD_LIBRARY_SPEEDMODIFIER_H
#define JAYD_LIBRARY_SPEEDMODIFIER_H

#include "Generator.h"
#include "Source.h"

#include <Buffer/DataBuffer.h>

class SpeedModifier : public Generator {

public:

	SpeedModifier(Generator* source);
	~SpeedModifier();

	size_t generate(int16_t* outBuffer) override;
	int available() override;

	/**
	 * Set speed multiplier as modifier. Will get mapped from 0-255 to 0.5 - 2.0
	 * @param modifier
	 */
	void setModifier(uint8_t modifier);

	/**
	 * Set speed multiplier.
	 * @param speed
	 */
	void setSpeed(float speed);

	void setSource(Generator* source);

	void resetBuffers();

private:
	Generator *source = nullptr;
	DataBuffer* dataBuffer = nullptr;

	float speed = 1;
	float remainder = 0;

	void fillBuffer();
};


#endif //JAYD_LIBRARY_SPEEDMODIFIER_H
