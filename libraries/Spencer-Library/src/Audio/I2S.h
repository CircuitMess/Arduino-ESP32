#ifndef SPENCER_I2S_H
#define SPENCER_I2S_H
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2s.h"
#include "esp_system.h"

class I2S {
public:
	I2S();
	void begin();
	void stop();
	int Read(char* data, int numData);
	bool isInited();
private:
	bool inited = 0;
};

#endif
