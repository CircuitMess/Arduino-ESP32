#include "Armstrong.h"
#include <Loop/LoopManager.h>
#include <SPIFFS.h>
#include <Wire.h>

ArmstrongImpl Armstrong;
EncoderInput Encoders;
ServoControl Motors;
LEDController LED;
RGBController RGB;
ColorSensor Sensor;

ArmstrongImpl::ArmstrongImpl(){

}

void ArmstrongImpl::begin(){
	if(!SPIFFS.begin(true)){
		printf("SPIFFS error\n");
	}

	Wire.begin(I2C_SDA, I2C_SCL);

	Encoders.begin();
	LED.begin();
	RGB.begin();
	Sensor.begin();
	Motors.begin();

	input = new InputShift(SHIFTIN_DATA, SHIFTIN_CLOCK, SHIFTIN_LOAD, 8);
	input->begin();
	LoopManager::addListener(input);
}

InputShift* ArmstrongImpl::getInput(){
	return input;
}

Slot ArmstrongImpl::btnToSlot(uint8_t i){
	auto pair = BtnSLotMap.find(i);
	if(pair == BtnSLotMap.end()) return None;

	return pair->second;
}
