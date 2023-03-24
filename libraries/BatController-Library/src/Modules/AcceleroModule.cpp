#include "AcceleroModule.h"
#include <Wire.h>


AcceleroModule::AcceleroModule() : Module(0x18){
}


void AcceleroModule::init(){
	const uint8_t initData[2] = { 0x20, 0b01010001 };

	Wire.beginTransmission(address);
	Wire.write(initData, sizeof(initData));
	const auto error = Wire.endTransmission();

	if(error != 0) errorOccured();
}

void AcceleroModule::transmission(){
	Wire.beginTransmission(address);
	Wire.write(0x28);
	if(Wire.endTransmission() != 0){
		errorOccured();
		return;
	}

	uint8_t data[6];
	Wire.requestFrom(address, (uint8_t) 6);
	Wire.readBytes(data, 6);

	accel = {
			(data[1] << 8) | data[0],
			(data[3] << 8) | data[2],
			(data[5] << 8) | data[4]
	};


	Wire.beginTransmission(address);
	Wire.write(0x26);
	if(Wire.endTransmission() != 0){
		errorOccured();
		return;
	}

	Wire.requestFrom(address, (uint8_t) 1);
	Wire.readBytes(reinterpret_cast<char*>(&temp), 1);

	temp += 25;
}

const glm::vec<3, int16_t>& AcceleroModule::getAccelerometer() const{
	return accel;
}

int8_t AcceleroModule::getTemperature() const{
	return temp;
}


