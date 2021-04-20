#include <WiFi.h>
#include <SPI.h>
#include <SD.h>
#include <SPIFFS.h>
#include <Loop/LoopManager.h>
#include "JayD.h"
#include "Settings.h"
#include "Services/SDScheduler.h"
#include "Input/InputJayD.h"

const i2s_pin_config_t i2s_pin_config = {
		.bck_io_num = I2S_BCK,
		.ws_io_num = I2S_WS,
		.data_out_num = I2S_DO,
		.data_in_num = I2S_DI
};

LEDmatrixImpl LEDmatrix;
MatrixManager matrixManager(&LEDmatrix);

JayDImpl JayD;

JayDImpl::JayDImpl() : display(160, 128, -1, -1){ }

void JayDImpl::begin(){
	if(psramFound()){
		Serial.printf("PSRAM init: %s, free: %d B\n", psramInit() ? "Yes" : "No", ESP.getFreePsram());
	}else{
		Serial.println("No PSRAM detected");
	}

	disableCore0WDT();
	disableCore1WDT();

	WiFi.mode(WIFI_OFF);
	btStop();

	SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_SS);
	SPI.setFrequency(60000000);
	if(!SD.begin(SD_CS, SPI)){
		Serial.println("No SD card");
		//for(;;);
	}
	if(!SPIFFS.begin()){
		Serial.println("SPIFFS error");
	}

	display.begin();
	SPI.setFrequency(20000000);

	if(!LEDmatrix.begin(I2C_SDA, I2C_SCL)){
		Serial.println("couldn't start matrix");
		for(;;);
	}

	LoopManager::addListener(&Sched);
	LoopManager::addListener(&matrixManager);
	LoopManager::addListener(new InputJayD());
	InputJayD::getInstance()->begin();

	Settings.begin();
	LEDmatrix.setBrightness(80.0f * (float) Settings.get().brightnessLevel / 255.0f);
}

Display& JayDImpl::getDisplay(){
	return display;
}
