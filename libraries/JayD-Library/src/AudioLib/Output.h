#ifndef JAYD_OUTPUT_H
#define JAYD_OUTPUT_H

#include <Arduino.h>
#include <Loop/LoopListener.h>
class OutputSplitter;
class Generator;

class Output : public LoopListener
{
	friend OutputSplitter;
public:
	Output(bool timed = false);
	~Output();
	void setSource(Generator* gen);
	void loop(uint _time) override;
	void stop();
	void start();

	void setGain(float gain);
	float getGain();

	bool isRunning();

protected:
	virtual void output(size_t numSamples) = 0;
	int16_t *inBuffer = nullptr;
	Generator* generator;

	virtual void init() = 0;
	virtual void deinit() = 0;

private:
	float gain = 1.0; //0 - 1.0
	uint32_t lastSample = 0;
	size_t receivedSamples = 0;
	bool timed = false;
	bool running = false;
};


#endif