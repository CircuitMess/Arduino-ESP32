#ifndef JAYD_H
#define JAYD_H

#define PIN_BL 25
#define SD_CS 22

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

#define I2S_WS 4
#define I2S_DO 14
#define I2S_BCK 21
#define I2S_DI -1

#define I2C_SDA 26
#define I2C_SCL 27

#define SPI_SCK 18
#define SPI_MISO 19
#define SPI_MOSI 23
#define SPI_SS -1

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
#include <SD.h>
#include "Settings.h"
#include "Services/SDScheduler.h"
#include "Input/InputJayD.h"
#include "AudioLib/Systems/MixSystem.h"
#include "AudioLib/Systems/PlaybackSystem.h"
#include <Devices/Matrix/IS31FL3731.h>

extern const i2s_pin_config_t i2s_pin_config;
extern Matrix LEDmatrix;
extern MatrixManager matrixManager;
extern IS31FL3731 charlie;

class JayDImpl {
public:
	JayDImpl();

	void begin();

	Display& getDisplay();

private:
	Display display;

};

extern JayDImpl JayD;

#endif