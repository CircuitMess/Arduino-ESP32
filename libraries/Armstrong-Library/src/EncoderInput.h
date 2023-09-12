#ifndef ARMSTRONG_LIBRARY_ENCODERINPUT_H
#define ARMSTRONG_LIBRARY_ENCODERINPUT_H

#include <Loop/LoopListener.h>
#include <Util/WithListeners.h>
#include <unordered_map>
#include <deque>
#include <mutex>
#include "Pins.hpp"
#include "Names.h"
#include <Util/Task.h>

class EncoderInput;

class EncoderListener {
friend EncoderInput;
private:
	virtual void encoderMove(Motor enc, int8_t amount) = 0;
};

class EncoderInput : public LoopListener, public WithListeners<EncoderListener> {
public:
	EncoderInput();
	void begin();
	void end();

private:
	void loop(uint micros) override;
	void scan(Motor enc);

	const std::unordered_map<Motor, std::pair<int, int>, MotorHash> PinMap = {
			{ Motor::Rotate, { ENC_1A, ENC_1B } },
			{ Motor::Extend, { ENC_2A, ENC_2B } },
			{ Motor::Pinch, { ENC_3A, ENC_3B } },
			{ Motor::Lift, { ENC_4A, ENC_4B } }
	};

	std::unordered_map<Motor, int32_t, MotorHash> prevState;

	struct Action {
		Motor enc;
		int8_t amount;
	};

	std::deque<Action> actionQueue;
	std::mutex queueMut;
	Task scanTask;
	void scanTaskFunc();

	uint32_t sendTimer = 0;
	static constexpr uint32_t SendInterval = 10000; // [us]

};


#endif //ARMSTRONG_LIBRARY_ENCODERINPUT_H
