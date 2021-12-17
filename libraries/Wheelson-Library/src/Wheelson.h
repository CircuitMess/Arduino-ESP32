#ifndef WHEELSON_LIBRARY_WHEELSON_H
#define WHEELSON_LIBRARY_WHEELSON_H

#include <Arduino.h>
#include <CircuitOS.h>
#include <Display/Display.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <Loop/LoopManager.h>
#include "BatteryService.h"
#include "Nuvoton/Nuvoton.h"
#include "Nuvoton/WheelsonInput.h"
#include "Nuvoton/WheelsonLED.h"
#include "Nuvoton/WheelsonMotor.h"
#include "Markers.h"
#include "BallTracker.h"
#include "Camera.h"
#include "Settings.h"

#define I2C_SDA 14
#define I2C_SCL 15

#define BTN_UP 3
#define BTN_DOWN 1
#define BTN_LEFT 0
#define BTN_RIGHT 2
#define BTN_MID 4
#define BTN_BACK 5

#define MOTOR_FL 0
#define MOTOR_BL 1
#define MOTOR_FR 2
#define MOTOR_BR 3

// camera pins
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      4
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#define PIN_JIG 35

extern Nuvoton Nuvo;
extern WheelsonLED LED;
extern WheelsonMotor Motors;
extern BatteryService Battery;

class WheelsonImpl {
public:
	WheelsonImpl();
	void begin();

	Display& getDisplay();
	Input* getInput();

private:
	Display display;
	Input* input = nullptr;

};

extern WheelsonImpl Wheelson;

#endif //WHEELSON_LIBRARY_WHEELSON_H
