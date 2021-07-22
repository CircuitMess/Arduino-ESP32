#include "WheelsonMotor.h"
#include "../Wheelson.h"
#include "../Settings.h"

WheelsonMotor::WheelsonMotor() : i2c(Nuvo.getI2C()){
}

void WheelsonMotor::setMotor(uint8_t id, int8_t intensity){
	if(id > 3) return;
	if(state[id] == intensity) return;
	if(intensity <= -128){
		intensity = -127;
	}
	state[id] = intensity;

	uint8_t msg[] = { MOTOR_SET_BYTE, id, static_cast<uint8_t>(intensity * Settings.get().speedMultiplier / 255) };
	i2c.send(msg, 3);
}

int8_t WheelsonMotor::getMotor(uint8_t id){
	if(id > 3) return 0;

	uint8_t msg[] = { MOTOR_GET_BYTE, id };
	i2c.request(msg, 2, &state[id], 1);

	return (state[id] * 255 / (int16_t) Settings.get().speedMultiplier);
}

void WheelsonMotor::stopAll(){
	for(int i = 0; i < 4; i++){
		setMotor(i, 0);
	}
}
