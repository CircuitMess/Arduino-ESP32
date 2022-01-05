#ifndef JAYD_OUTPUTDAC_H
#define JAYD_OUTPUTDAC_H

#include "Output.h"
#include "../Setup.hpp"
#include <driver/i2s.h>
#include <Buffer/DataBuffer.h>

class OutputDAC : public Output
{
public:
	OutputDAC(int pin_out, int pin_sd = -1);
	~OutputDAC();

	void init() override;
	void deinit() override;

	static void IRAM_ATTR timerInterrupt();

protected:
	void output(size_t numBytes) override;

private:
	int PIN_SD = -1;
	int PIN_OUT = -1;

	DataBuffer buffer;

	uint8_t* outputBuffer[DAC_BUFFERS] = { nullptr };
	bool bufferStatus[DAC_BUFFERS] = { false }; // True - buffer is filled
	uint32_t bufferSamples[DAC_BUFFERS] = { 0 };
	uint8_t currentBuffer = 0;

	void transferBuffer();

	uint32_t currentSample = BUFFER_SAMPLES;

	hw_timer_t* timer = nullptr;
};


#endif