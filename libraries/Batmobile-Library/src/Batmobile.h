#ifndef BATMOBILE_LIBRARY_BATMOBILE_H
#define BATMOBILE_LIBRARY_BATMOBILE_H

#include <Arduino.h>
#include <CircuitOS.h>
#include <BatCommon.h>
#include <CMAudio.h>
#include <Display/Display.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <Loop/LoopManager.h>
#include "S3Interface.h"
#include "Motors.h"
#include "AudioSystem.h"
#include "LEDController.h"
#include "Communication/Communication.h"
#include "Battery.h"

class BatmobileImpl {
public:
	BatmobileImpl();
	void begin();

	/**
	 * Powers off all the system included in the library and puts the ESP into deep sleep.
	 */
    void shutdown();

	/**
	 * Plays /SFX/disconnect.aac, fades out underlights, and calls shutdown()
	 */
	void shutdownNotify();
	void shutdownError();

private:


};

extern SingleLEDController Headlights;
extern SingleLEDController Taillights;
extern RGBLEDController Underlights;

extern BatmobileImpl Batmobile;
extern MotorControl Motors;
extern BatteryService Battery;
extern S3Interface S3;
extern AudioSystem Audio;


#endif //BATMOBILE_LIBRARY_BATMOBILE_H
