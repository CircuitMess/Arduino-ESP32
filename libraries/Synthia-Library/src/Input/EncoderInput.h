#ifndef SYNTHIA_LIBRARY_ENCODERINPUT_H
#define SYNTHIA_LIBRARY_ENCODERINPUT_H

#include <Loop/LoopListener.h>
#include "../WithListeners.h"

class EncoderInput;

class EncoderListener {
friend EncoderInput;

private:
	virtual void leftEncMove(int8_t amount);
	virtual void rightEncMove(int8_t amount);

};

class EncoderInput : public LoopListener, public WithListeners<EncoderListener> {
public:
	EncoderInput();

	void begin();
	void loop(uint time) override;

	void setLeftEncCallback(void (* leftEncListener)(int8_t));
	void setRightEncCallback(void (* rightEncListener)(int8_t));

private:
	int prevStateLeft = INT32_MAX;
	int prevStateRight = INT32_MAX;

	void (*leftEncCallback)(int8_t value) = nullptr;
	void (*rightEncCallback)(int8_t value) = nullptr;

};

#endif //SYNTHIA_LIBRARY_ENCODERINPUT_H
