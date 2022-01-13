#include "OutputI2S.h"
#include "../Setup.hpp"
#include "../PerfMon.h"

OutputI2S::OutputI2S(i2s_config_t config, i2s_pin_config_t pins, i2s_port_t port) : Output(true), config(config), pins(pins), port(port){
}

OutputI2S::~OutputI2S(){
}

void OutputI2S::output(size_t numSamples){
	size_t bytesWritten;
	Profiler.start("I2S write");
	i2s_write_expand(I2S_NUM_0, inBuffer, numSamples*NUM_CHANNELS*BYTES_PER_SAMPLE, BYTES_PER_SAMPLE * 8, 32, &bytesWritten, portMAX_DELAY);
	Profiler.end();
}

void OutputI2S::init(){
	esp_err_t err = i2s_driver_install(port, &config, 0, NULL);
	if (err != ESP_OK) {
		Serial.printf("Failed installing I2S driver: %d\n", err);
	}
	err = i2s_set_pin(I2S_NUM_0, &pins);
	if (err != ESP_OK) {
		Serial.printf("Failed setting I2S pin: %d\n", err);
	}
}

void OutputI2S::deinit(){
	esp_err_t err = i2s_driver_uninstall(port);
	if (err != ESP_OK) {
		Serial.printf("Failed uninstalling I2S driver: %d\n", err);
	}
}