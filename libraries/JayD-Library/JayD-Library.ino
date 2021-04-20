#include <Arduino.h>
#include <CircuitOS.h>
#include "src/JayD.hpp"
#include <Loop/LoopManager.h>
#include "src/AudioLib/SourceWAV.h"
#include "src/AudioLib/OutputI2S.h"
#include "src/AudioLib/Mixer.h"
#include "src/PerfMon.h"
#include "src/Services/SDScheduler.h"
#include "src/Input/InputJayD.h"
#include "src/Settings.h"
#include "src/AudioLib/Systems/MixSystem.h"

#include <Display/Display.h>
#include <SPI.h>
#include <Devices/LEDmatrix/LEDmatrix.h>
#include <Devices/SerialFlash/SerialFlashFileAdapter.h>
#include <SD.h>
#include <WiFi.h>
#include <Util/Task.h>
#include <esp_partition.h>
#include <SPIFFS.h>

Display display(160, 128, -1, -1);

int pixel = 0;

void lightPixel(int pixel, bool turnoff = false){
	if(turnoff){
		LEDmatrix.drawPixel(::pixel, 0);
	}

	pixel = max(pixel, 0);
	pixel = min(pixel, 16 * 9 - 1);

	LEDmatrix.drawPixel(pixel, 255);

	::pixel = pixel;
}

void recurseDir(File dir){
	File f;
	while(f = dir.openNextFile()){
		if(f.isDirectory()){
			recurseDir(f);
			continue;
		}

		Serial.println(f.name());
		f.close();
	}
}

void listSD(){
	File root = SD.open("/");
	if(!root){
		Serial.println("List SD: can't open root");
		return;
	}

	recurseDir(root);
	root.close();
}

MixSystem* mix = nullptr;

void setup(){
	Serial.begin(115200);

	pinMode(25, OUTPUT);
	digitalWrite(25, HIGH);

	WiFi.mode(WIFI_OFF);
	btStop();

	esp_chip_info_t chip_info;
	esp_chip_info(&chip_info);
	printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
		   chip_info.cores,
		   (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
		   (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

	printf("silicon revision %d, ", chip_info.revision);

	printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
		   (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

	fflush(stdout);

	if(psramFound()){
		Serial.printf("PSRAM init: %s, free: %d B\n", psramInit() ? "Yes" : "No", ESP.getFreePsram());
	}else{
		Serial.println("No PSRAM");
	}

	SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_SS);
	SPI.setFrequency(60000000);
	if(!SD.begin(22, SPI)){
		Serial.println("No SD card");
		for(;;);
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

	Settings.begin();
	LEDmatrix.setBrightness(80.0f * (float) Settings.get().brightnessLevel / 255.0f);

	LoopManager::addListener(&Sched);
	LoopManager::addListener(&matrixManager);
	LoopManager::addListener(new InputJayD());
	InputJayD::getInstance()->begin();

	mix = new MixSystem(SD.open("/Walter2.aac"), SD.open("/Recesija2.aac"));
	mix->setMix(128);
	mix->setVolume(0, 50);
	mix->setVolume(1, 50);
	mix->start();

	/*InputJayD* input = new InputJayD();
	input->begin();
	LoopManager::addListener(input);

	digitalWrite(JDNV_PIN_RESET, LOW);*/

	/*LEDMatrix.begin(26, 27);
	LEDMatrix.clear();
	LEDMatrix.push();

	InputJayD::getInstance()->setEncoderMovedCallback(1, [](int8_t value){
		lightPixel(pixel + value, true);
		LEDMatrix.push();

		Serial.printf("%d\n", pixel);
	});*/
}

int i = 0;
void loop(){
	LoopManager::loop();

	/*i++;

	LEDMatrix.clear();
	lightPixel((i/13) % 144);
	lightPixel((i/17) % 144);
	lightPixel((i/5) % 144);
	lightPixel((i/7) % 144);
	lightPixel((i/11) % 144);
	LEDMatrix.push();*/
}