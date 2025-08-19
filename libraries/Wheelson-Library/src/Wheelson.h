#ifndef WHEELSON_LIBRARY_WHEELSON_H
#define WHEELSON_LIBRARY_WHEELSON_H

#include <Arduino.h>
#include <CircuitOS.h>
#include <Display/Display.h>
#include "WheelsonDisplay.h"
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
#include "Pins.hpp"
#include <Util/PinMap.h>

extern Nuvoton Nuvo;
extern WheelsonLED LED;
extern WheelsonMotor Motors;
extern BatteryService Battery;
extern PinMap<Pin> Pins;

class WheelsonImpl {
public:
	WheelsonImpl();
	void begin();

	Display& getDisplay();
	Input* getInput();

	void initVer(int override = -1); // Initializes version and pins; also called from begin()

	enum class Ver {
		v1, v2, v3
	} ver = Ver::v1;

private:
	Display display;
	Input* input = nullptr;

	bool verInited = false;
};

extern WheelsonImpl Wheelson;

#endif //WHEELSON_LIBRARY_WHEELSON_H
