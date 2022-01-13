#include "ByteBoi.h"
#include <SPIFFS.h>
#include <SD.h>
#include <SPI.h>
#include <esp_partition.h>
#include <esp_ota_ops.h>
#include <Loop/LoopManager.h>
#include "Menu/Menu.h"
#include "Battery/BatteryPopupService.h"
#include "SleepService.h"
#include "Bitmaps/ByteBoiLogo.hpp"
#include <Loop/LoopManager.h>
#include <esp_wifi.h>
#include <esp_bt.h>
#include <driver/adc.h>

const char* ByteBoiImpl::SPIFFSgameRoot = "/game";
const char* ByteBoiImpl::SPIFFSdataRoot = "/data";
using namespace std;
using namespace MiniMenu;

ByteBoiImpl ByteBoi;
BatteryService Battery;
BatteryPopupService BatteryPopup;
Menu* ByteBoiImpl::popupMenu = nullptr;

void ByteBoiImpl::begin(){
	pinMode(SPEAKER_SD, OUTPUT);
	digitalWrite(SPEAKER_SD, HIGH);
	delay(10);
	dacWrite(SPEAKER_PIN, 127);

	uint64_t mac = ESP.getEfuseMac();
	uint32_t upper = mac >> 32;
	uint32_t lower = mac & 0xffffffff;
	Serial.printf("ByteBoi:v1.0:%x%x\n", upper, lower);

	if(!inFirmware()){
		esp_ota_set_boot_partition(esp_ota_get_next_update_partition(esp_ota_get_running_partition()));
	}

	if(psramFound()){
		Serial.printf("PSRAM init: %s, free: %d B\n", psramInit() ? "Yes" : "No", ESP.getFreePsram());
	}else{
		Serial.println("No PSRAM detected");
	}

	if(!SPIFFS.begin()){
		Serial.println("SPIFFS error");
	}

	Settings.begin();

	Playback.begin();

	SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_SS);
	SPI.setFrequency(60000000);

	display = new Display(160, 120, -1, 1);
	expander = new I2cExpander();

	display->begin();
	display->getBaseSprite()->clear(TFT_BLACK);
	display->commit();

	expander->begin(I2C_EXPANDER_ADDR, I2C_SDA, I2C_SCL);
	expander->pinMode(BL_PIN, OUTPUT);
	expander->pinWrite(BL_PIN, 0);
	LED.begin();
	LED.setRGB(OFF);
	expander->pinMode(SD_DETECT_PIN, INPUT);
	expander->portRead();

	input = new InputI2C(expander);
	input->preregisterButtons({ BTN_A, BTN_B, BTN_C, BTN_UP, BTN_DOWN, BTN_RIGHT, BTN_LEFT });
	LoopManager::addListener(Input::getInstance());

	Context::setDeleteOnPop(true);

	Battery.begin();
}

String ByteBoiImpl::getSDPath(){
	if(isStandalone()) return "";

	const char* rootFilePath = "/launcher/gameRoot.path";
	fs::File rootFile = SPIFFS.open(rootFilePath);
	if(!rootFile) return "";

	char* root = static_cast<char*>(malloc(rootFile.size() + 1));
	rootFile.read(reinterpret_cast<uint8_t*>(root), rootFile.size());
	root[rootFile.size()] = 0;
	rootFile.close();

	String path(root);
	free(root);

	return path;
}

File ByteBoiImpl::openResource(const String& path, const char* mode){
	if(strcmp(esp_ota_get_running_partition()->label, "game") == 0){
		String result = String(SPIFFSgameRoot + path);
		if(!SPIFFS.exists(result)) return File();
		return SPIFFS.open(result, mode);
	}else{
		return SPIFFS.open(path, mode);
	}

}

File ByteBoiImpl::openData(const String& path, const char* mode){
	if(gameID.length() == 0) return File(); //undefined game ID
	return SPIFFS.open(String(SPIFFSdataRoot) + "/" + gameID + path, mode);
}

bool ByteBoiImpl::removeData(const String& path){
	if(gameID.length() == 0) return false; //undefined game ID
	String spiffsPath = String(SPIFFSdataRoot) + "/" + gameID + path;
	if(!SPIFFS.exists(spiffsPath)) return false;
	return SPIFFS.remove(spiffsPath);
}

bool ByteBoiImpl::inFirmware(){
	return (strcmp(esp_ota_get_running_partition()->label, "launcher") == 0); //already in launcher partition
}

bool ByteBoiImpl::isStandalone(){
	return (strcmp(esp_ota_get_running_partition()->label, "game") != 0);
}

void ByteBoiImpl::backToLauncher(){
	if(inFirmware() || isStandalone()) return;

	fadeout();
	expander->pinWrite(BL_PIN, HIGH);

	const esp_partition_t *partition = esp_ota_get_running_partition();
	const esp_partition_t *partition2 = esp_ota_get_next_update_partition(partition);
	esp_ota_set_boot_partition(partition2);
	ESP.restart();
}

Display* ByteBoiImpl::getDisplay(){
	return display;
}

I2cExpander* ByteBoiImpl::getExpander(){
	return expander;
}

InputI2C* ByteBoiImpl::getInput(){
	return input;
}

void ByteBoiImpl::setGameID(String ID){
	gameID = ID.substring(0, 5);
}

void ByteBoiImpl::bindMenu(){
	menuBind = true;
	input->addListener(this);
}

void ByteBoiImpl::unbindMenu(){
	menuBind = false;
	input->removeListener(this);
}

void ByteBoiImpl::buttonPressed(uint i){
	if(!menuBind){
		input->removeListener(this);
		return;
	}

	if(i == BTN_C || (popupMenu != nullptr && i == BTN_B)){
		if(ContextTransition::isRunning() || ModalTransition::isRunning()) return;

		if(popupMenu == nullptr){
			openMenu();
		}else if(popupMenu != nullptr){
			Menu::popIntoPrevious();
			popupMenu = nullptr;
		}
	}
}

void ByteBoiImpl::openMenu(){
	if(ContextTransition::isRunning() || ModalTransition::isRunning()) return;
	if(Modal::getCurrentModal() != nullptr){
		ModalTransition::setDeleteOnPop(false);
		auto transition = static_cast<ModalTransition *>((void *)Modal::getCurrentModal()->pop());
		transition->setDoneCallback([](Context *currentContext, Modal *prevModal){
			ByteBoiImpl::popupMenu = new Menu(currentContext);
			ByteBoiImpl::popupMenu->push(currentContext);
			ByteBoiImpl::popupMenu->returned(prevModal);
			ModalTransition::setDeleteOnPop(true);
		});
	}else{
		ByteBoiImpl::popupMenu = new Menu(Context::getCurrentContext());
		ByteBoiImpl::popupMenu->push(Context::getCurrentContext());
	}
}

void ByteBoiImpl::fadeout(){
	Sprite* canvas = display->getBaseSprite();

	Color* logoBuffer = static_cast<Color*>(malloc(93*26*2));
	memcpy_P(logoBuffer,logoIcon,93*26*2);
	canvas->clear(C_HEX(0x0041ff));
	if(logoBuffer) canvas->drawIcon(logoBuffer, (display->getWidth() - 93) / 2, (display->getHeight() - 26) / 2, 93, 26);
	display->commit();
	delay(1000);

	auto color = canvas->readPixelRGB(0, 0);
	lgfx::rgb565_t pixel(0xffff);
	while(color.B8() > 0){
		color.set(color.R8() * 0.7, color.G8() * 0.7, color.B8() * 0.7);

		canvas->clear(C_RGB(color.R8(), color.G8(), color.B8()));
		if(logoBuffer){
			pixel.set(pixel.R8() * 0.7, pixel.G8() * 0.7, pixel.G8() * 0.7);
			Color c = pixel.operator unsigned short();
			for(int i = 0; i < 93 * 26; i++){
				if(logoBuffer[i] != TFT_TRANSPARENT){
					logoBuffer[i] = c;
				}
			}
			canvas->drawIcon(logoBuffer, (display->getWidth() - 93) / 2, (display->getHeight() - 26) / 2, 93, 26);
		}

		if(color.B8() < 3) color.set(0, 0, 0);
		display->commit();
	}
	free(logoBuffer);
	canvas->clear(TFT_BLACK);
	display->commit();
}

void ByteBoiImpl::shutdown(){
	display->getTft()->sleep();
	expander->pinWrite(BL_PIN, HIGH);
	LED.setRGB(OFF);
	Playback.stop();
	delay(100);
	digitalWrite(SPEAKER_SD, HIGH);
#ifndef BYTEBOI_LAUNCHER
	btStop();
	esp_bt_controller_disable();
	esp_wifi_stop();
#endif
	adc_power_off();
	esp_deep_sleep_start();
}

void ByteBoiImpl::splash(void(* callback)()){
	display->getBaseSprite()->clear(C_HEX(0x0041ff));
	display->getBaseSprite()->drawIcon(logoIcon, (display->getWidth() - 93) / 2, (display->getHeight() - 26) / 2, 93, 26);
	display->commit();

	splashCallback = callback;
	splashTime = millis();
	LoopManager::addListener(this);

	if(splashCallback == nullptr){
		while(splashTime){
			loop(0);
		}
	}
}

void ByteBoiImpl::loop(uint micros){
	if(millis() - splashTime >= 1000){
		splashTime = millis();
		if(splashCallback != nullptr){
			void (* callback)() = splashCallback;
			splashCallback = nullptr;
			callback();
		}
		LoopManager::removeListener(this);
		splashTime = false;
	}

}



