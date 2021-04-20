#ifndef JAYD_GENERATOR_H
#define JAYD_GENERATOR_H
#include <Arduino.h>

class Generator
{
public:
	virtual ~Generator() = default;
	/**
	 * @brief Generates samples and copies them to the supplied buffer
	 * 
	 * @param outBuffer Specified buffer for the generated samples.
	 * @return size_t Number of samples generated.
	 */
	virtual size_t generate(int16_t *outBuffer) = 0;
	virtual int available() = 0;
};

#endif