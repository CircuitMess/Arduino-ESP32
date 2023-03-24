#include "Motors.h"
#include <Loop/LoopManager.h>
#include "Pins.hpp"

const uint8_t MotorControl::PWM[4] = { 5, 6, 7, 8 };

const std::pair<int8_t, int8_t> MotorControl::Pins[4] = {
		{ MOTOR_FL_A, MOTOR_FL_B },
		{ MOTOR_FR_A, MOTOR_FR_B },
		{ MOTOR_BL_A, MOTOR_BL_B },
		{ MOTOR_BR_B, MOTOR_BR_A }
};

void MotorControl::begin(){
	for(auto pin : { MOTOR_FL_A, MOTOR_FR_A, MOTOR_BL_A, MOTOR_BR_A, MOTOR_FL_B, MOTOR_FR_B, MOTOR_BL_B, MOTOR_BR_B }){
		pinMode(pin, OUTPUT);
		digitalWrite(pin, HIGH);
	}

	for(int i = 0; i < 4; i++){
		ledcSetup(PWM[i], 1000, 8);
		ledcWrite(PWM[i], 255);
		ledcAttachPin(Pins[i].first, PWM[i]);
	}

	stopAll();
	for(int i = 0; i < 4; i++){
		sendMotorPWM((Motor) i, 0);
	}

	LoopManager::addListener(this);
}

void MotorControl::end(){
	for(auto pin : { MOTOR_FL_A, MOTOR_FR_A, MOTOR_BL_A, MOTOR_BR_A, MOTOR_FL_B, MOTOR_FR_B, MOTOR_BL_B, MOTOR_BR_B }){
		ledcDetachPin(pin);
		pinMode(pin, OUTPUT);
		digitalWrite(pin, LOW);
	}
}

void MotorControl::setFR(int8_t value){
	setMotorTarget(FrontRight, value);
}

void MotorControl::setFL(int8_t value){
	setMotorTarget(FrontLeft, value);
}

void MotorControl::setBR(int8_t value){
	setMotorTarget(BackRight, value);
}

void MotorControl::setBL(int8_t value){
	setMotorTarget(BackLeft, value);
}

void MotorControl::setRight(int8_t value){
	setMotorTarget(FrontRight, value);
	setMotorTarget(BackRight, value);
}

void MotorControl::setLeft(int8_t value){
	setMotorTarget(FrontLeft, value);
	setMotorTarget(BackLeft, value);
}

void MotorControl::setAll(int8_t value){
	setAll({ value, value, value, value });
}

void MotorControl::setAll(MotorInfo state){
	setFR(state.frontRight);
	setFL(state.frontLeft);
	setBR(state.backRight);
	setBL(state.backLeft);
}

MotorInfo MotorControl::getAll() const{
	return stateTarget.val;
}

void MotorControl::stopAll(){
	setAll({ 0, 0, 0, 0 });
}

void MotorControl::setMotorTarget(Motor motor, int8_t value){
	size_t i = motor;
	if(i >= 4) return;

	value = constrain(value, -100, 100);
	stateTarget.raw[i] = value;
}

void MotorControl::sendMotorPWM(Motor motor, int8_t value){
	size_t i = motor;
	if(i >= 4) return;

	const auto& pins = Pins[i];

	if(value == 0){
		ledcWrite(PWM[i], 255);
		// ledcDetachPin(PWM[i]);
		digitalWrite(pins.first, HIGH);
		digitalWrite(pins.second, HIGH);
		return;
	}

	// motor driver is a basic H bridge and has two input lines (A and B) for each motor
	// Ab - forward, aB - backward, AB - brake, ab - no power
	// going forward: pull B low, drive A with PWM
	// going backward: put B high, drive A with reverse PWM value

	// value is [-100, 100], duty is [0, 255]
	const bool reverse = (value < 0) ^ (motor == FrontRight || motor == BackRight); // Right side motors are reverse
	value = abs(value);
	auto duty = (uint8_t) std::round(((double) value * 255.0) / 100.0);

	if(reverse){
		digitalWrite(pins.second, HIGH);
		duty = 255 - duty;
	}else{
		digitalWrite(pins.second, LOW);
	}

	// ledcAttachPin(pins.first, PWM[i]);
	ledcWrite(PWM[i], duty);
}

void MotorControl::loop(uint micros){
	easeCounter += micros;
	if(easeCounter < EaseInterval) return;
	easeCounter = 0;

	const double dt = (double) EaseInterval / 1000000.0;

	for(int i = 0; i < 4; i++){
		if(stateTarget.raw[i] == stateActual[i]) continue;
		double direction = (stateTarget.raw[i] > stateActual[i]) ? 1 : -1;

		double oldValue = stateActual[i];
		double newValue = constrain(oldValue + direction * EaseValue * dt, -100.0, 100.0);

		if(direction > 0){
			newValue = std::min(newValue, (double) stateTarget.raw[i]);
		}else{
			newValue = std::max(newValue, (double) stateTarget.raw[i]);
		}

		stateActual[i] = newValue;

		if(std::round(newValue) != std::round(oldValue)){
			sendMotorPWM((Motor) i, (int8_t) std::round(newValue));
		}
	}
}
