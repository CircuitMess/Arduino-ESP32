#ifndef WHEELSON_LIBRARY_WHEELSONMOTOR_H
#define WHEELSON_LIBRARY_WHEELSONMOTOR_H

#include <Wire.h>
#include <Sync/Mutex.h>
#include "I2CQueue.h"

#define MOTOR_SET_BYTE 0X30
#define MOTOR_GET_BYTE 0X31

class WheelsonMotor {
public:
	WheelsonMotor();
	void setMotor(uint8_t id, int8_t intensity);
	int8_t getMotor(uint8_t id);
	void stopAll();

private:
	I2CQueue& i2c;
	int8_t state[4] = {0};

};


#endif //WHEELSON_LIBRARY_WHEELSONMOTOR_H
