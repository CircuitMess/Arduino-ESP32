#include <Arduino.h>
#include <CircuitOS.h>
#include <SPI.h>
#include <Loop/LoopManager.h>
#include <SerialFlash.h>
#include <CircuitOS.h>
#include <Input/InputGPIO.h>
#include <Devices/Matrix/IS31FL3731.h>
#include <Devices/Matrix/Matrix.h>
#include <Devices/SerialFlash/SerialFlashFileAdapter.h>
#include <Devices/Matrix/MatrixAnimGIF.h>
#include <WiFi.h>
#include <Network/Net.h>
#include <Util/Task.h>
#include "Settings.h"
#include "Audio/Playback.h"
#include "Audio/Recording.h"
#include "PreparedStatement.h"
#include "Speech/SpeechToIntent.h"
#include "Speech/TextToSpeech.h"

#ifndef SPENCER_HPP
#define SPENCER_HPP

#define NEOPIXEL_PIN 2
#define FLASH_CS_PIN 5
#define BTN_PIN 17
#define LED_PIN 26
#define I2C_SDA 22
#define I2C_SCL 21

class SpencerImpl
{
public:
	SpencerImpl();
	~SpencerImpl();

	void begin();

	/**
	 * Loads Spencer's settings. If no settings are stored, i.e. this is the first time Spencer is started, default values will be set and stored.
	 * @return True if no settings are stored (if Spencer hasn't been started before)
	 */
	bool loadSettings();
};

extern Matrix LEDmatrix;
extern SpencerImpl Spencer;
#endif
