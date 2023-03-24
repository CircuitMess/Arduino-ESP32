#ifndef BATMOBILE_LIBRARY_MOTORS_H
#define BATMOBILE_LIBRARY_MOTORS_H

#include <Arduino.h>
#include <Loop/LoopListener.h>
#include "Pins.hpp"
#include <DriveInfo.h>

class MotorControl : private LoopListener {
public:
	void begin();
	void end();

	void setFR(int8_t value);
	void setFL(int8_t value);
	void setBR(int8_t value);
	void setBL(int8_t value);
	void setRight(int8_t value);
	void setLeft(int8_t value);
	void setAll(int8_t value);

	void setAll(MotorInfo state);
	MotorInfo getAll() const;

	void stopAll();

private:
	enum Motor : uint8_t { FrontLeft, FrontRight, BackLeft, BackRight };

	static const uint8_t PWM[4];
	static const std::pair<int8_t, int8_t> Pins[4];

	union {
		MotorInfo val;
		int8_t raw[4];
	} stateTarget = { .val = { 0, 0, 0, 0 } };

	double stateActual[4] = { 0, 0, 0, 0 };

	void setMotorTarget(Motor motor, int8_t value);
	void sendMotorPWM(Motor motor, int8_t value);

	void loop(uint micros) override;

	static constexpr double EaseValue = 200; // value change per second
	static constexpr uint32_t EaseInterval = 10000; // adjust PWM value every EaseInterval microseconds
	uint32_t easeCounter = 0;

};


#endif //BATMOBILE_LIBRARY_MOTORS_H
