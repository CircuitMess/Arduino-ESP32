#ifndef JAYD_H
#define JAYD_H

#define ENC_MID 0
#define ENC_L1 1
#define ENC_L2 6
#define ENC_L3 5
#define ENC_R1 4
#define ENC_R2 3
#define ENC_R3 2

#define POT_L 1
#define POT_MID 0
#define POT_R 2

#define BTN_L 0
#define BTN_R 1
#define BTN_MID 2
#define BTN_L1 3
#define BTN_L2 8
#define BTN_L3 7
#define BTN_R1 6
#define BTN_R2 5
#define BTN_R3 4

#include <Arduino.h>
#include <CircuitOS.h>
#include <Loop/LoopManager.h>
#include <Display/Display.h>
#include <Devices/Matrix/Matrix.h>
#include <Devices/Matrix/MatrixAnimGIF.h>
#include "Matrix/MatrixManager.h"
#include <driver/i2s.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include <SPI.h>
#include "Settings.h"
#include "Services/SDScheduler.h"
#include "Input/InputJayD.h"
#include "AudioLib/Systems/MixSystem.h"
#include "AudioLib/Systems/PlaybackSystem.h"
#include <Devices/Matrix/IS31FL3731.h>
#include "JayDDisplay.h"

extern Matrix LEDmatrix;
extern MatrixManager matrixManager;
extern IS31FL3731 charlie;

class JayDImpl {
public:
	JayDImpl();
	void initVer(int override = -1); // Initializes version and pins; also called from begin()

	void begin();

	Display& getDisplay();

	File SD_open(const char* path, const char* mode = FILE_READ);
	File SD_open(String path, const char* mode = FILE_READ);

	bool SD_exists(const char* path);
	bool SD_exists(const String& path);
	bool SD_remove(const char* path);
	bool SD_remove(const String& path);

	bool SD_begin();
	void SD_end();

	i2s_pin_config_t i2s_pin_config;

private:
	Display display;

	enum class Ver { v1_0, v1_1, v1_2, v1_3 } ver = Ver::v1_0;

	bool verInited = false;
};

extern JayDImpl JayD;

#endif
