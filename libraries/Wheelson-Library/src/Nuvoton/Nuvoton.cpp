#include "Nuvoton.h"
#include "../Wheelson.h"

Nuvoton::Nuvoton(TwoWire& Wire) : Wire(Wire), i2c(WSNV_ADDR, this->Wire) {

}

bool Nuvoton::begin() {
	pinMode(WSNV_PIN_RESET, OUTPUT);
	reset();
	delay(50);

	i2c.setHome();

	Wire.begin(I2C_SDA, I2C_SCL);

	Wire.beginTransmission(WSNV_ADDR);
	if(Wire.endTransmission() != 0){
		return false;
	}
	return identify();
}

bool Nuvoton::identify() {
	uint8_t msg[] = { IDENTIFY_BYTE };
	uint8_t value;

	i2c.request(msg, 1, &value, 1);

	return value == WSNV_ADDR;
}

void Nuvoton::reset() {
	digitalWrite(WSNV_PIN_RESET, LOW);
	delay(50);
	digitalWrite(WSNV_PIN_RESET, HIGH);
}

uint16_t Nuvoton::getBatteryVoltage(){
	uint16_t level;

	uint8_t msg[] = { BATTERY_BYTE };
	i2c.request(msg, 1, &level, 2);

	return level;
}

TwoWire &Nuvoton::getWire() {
	return this->Wire;
}

void Nuvoton::shutdown(){
	for(uint8_t i = 0; i < 4; i++){
		Motors.setMotor(i, 0);
	}
	LED.setBacklight(false);
	LED.setHeadlight(0);
	LED.setRGB(OFF);

	uint8_t msg[] = { SHUTDOWN_BYTE };
	i2c.send(msg, 1);
}

I2CQueue& Nuvoton::getI2C(){
	return i2c;
}