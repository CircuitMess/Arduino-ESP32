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
#include "ByteBoi2Input.h"
#include <Loop/LoopManager.h>
#include <esp_wifi.h>
#include <esp_bt.h>
#include <driver/adc.h>
#include <Input/InputShift.h>
#include "Pins.hpp"
#include "PinDef.h"
#include <Util/HWRevision.h>

const char* ByteBoiImpl::SPIFFSgameRoot = "/game";
const char* ByteBoiImpl::SPIFFSdataRoot = "/data";
using namespace std;
using namespace MiniMenu;

ByteBoiImpl ByteBoi;
BatteryService Battery;
BatteryPopupService BatteryPopup;
Menu* ByteBoiImpl::popupMenu = nullptr;
PinMap<Pin> Pins;

void ByteBoiImpl::begin(){
	initVer();

	if(ver == v1_0){
		expander = new I2cExpander();
		expander->begin(0x74, I2C_SDA, I2C_SCL);

		expander->pinMode(BL_PIN, OUTPUT);
		expander->pinWrite(BL_PIN, HIGH);
		expander->pinMode(SD_DETECT_PIN, INPUT);
		expander->portRead();

		input = new InputI2C(expander);
	}else{
		auto* inputShift = new ByteBoi2Input(SHIFT_SDA, SHIFT_SCL, SHIFT_PL, 8);
		inputShift->begin();
		input = inputShift;

		pinMode(CHARGE_DETECT_PIN, INPUT);
		pinMode(BL_PIN, OUTPUT);
		digitalWrite(BL_PIN, HIGH);
	}

	LED.begin();
	LED.setRGB(OFF);

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

	display = new Display(160, 120, -1, 1);
	if(ver == v1_0){
		display->getTft()->setPanel(ByteBoiDisplay::panel1());
	}else if(ver == v1_1){
		display->getTft()->setPanel(ByteBoiDisplay::panel2());
	}else{
		display->getTft()->setPanel(ByteBoiDisplay::panel3());
	}

	display->begin();
	display->getBaseSprite()->clear(TFT_BLACK);
	display->commit();

	Battery.begin();
	if(Battery.getLevel() == 0 && !Battery.isCharging()){
		ByteBoi.shutdown();
	}

	//Audio thread #0 waits for resources loaded on thread #1
	disableCore0WDT();

	Settings.begin();

	Playback.begin();

	SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_SS);
	SPI.setFrequency(60000000);

	checkSD();

	input->preregisterButtons({ (uint8_t) BTN_A, (uint8_t) BTN_B, (uint8_t) BTN_C, (uint8_t) BTN_UP, (uint8_t) BTN_DOWN, (uint8_t) BTN_RIGHT, (uint8_t) BTN_LEFT });
	LoopManager::addListener(input);

	Context::setDeleteOnPop(true);

	setBacklight(true);
}

void ByteBoiImpl::initVer(int override){
	if(verInited) return;
	verInited = true;

	const auto hw = override == -1 ? HWRevision::get() : override;

	if(hw == 1){
		// HW v2
		ver = v2_0;
		Pins.set(Pins3);
	}else{
		// HW v1.0 I2C pins
		static constexpr int Sda = 23;
		static constexpr int Scl = 22;

		Wire.begin(Sda, Scl);
		Wire.beginTransmission(0x74);
		if(Wire.endTransmission() == 0){
			// Expander present, this is HW v1.0
			ver = v1_0;
			Pins.set(Pins1);
		}else{
			// Expander not present, this is HW v1.1
			ver = v1_1;
			Pins.set(Pins2);
		}
	}
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
	setBacklight(false);

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

Input* ByteBoiImpl::getInput(){
	return input;
}

void ByteBoiImpl::checkSD(){
	if(expander){
		bool wasInserted = sdInserted;
		sdInserted = !(ByteBoi.getExpander()->getPortState() & (1 << SD_DETECT_PIN));

		if(wasInserted && !sdInserted){
			SD.end();
		}else if(!wasInserted && sdInserted){
			sdInserted = SD.begin(SD_CS, SPI);

			if(!sdInserted) return;

			if(!SD.exists("/.ByteBoi")){
				File f = SD.open("/.ByteBoi", FILE_WRITE);
				f.write(0);
				f.close();
			}
		}
	}else{
		if(sdInserted){
			File f = SD.open("/.ByteBoi");
			uint8_t b;
			if(f && f.read(&b, 1)){
				sdInserted = true;
				return;
			}
			f.close();
			SD.end();
			sdInserted = false;
		}else{
			if(!SD.begin(SD_CS, SPI)){
				sdInserted = false;
				return;
			}
			if(!SD.exists("/.ByteBoi")){
				File f = SD.open("/.ByteBoi", FILE_WRITE);
				f.write(0);
				f.close();
			}
			sdInserted = true;
		}
	}
}

bool ByteBoiImpl::sdDetected(){
	if(expander){
		checkSD();
		return sdInserted;
	}else{
		return sdInserted;
	}
}

void ByteBoiImpl::setBacklight(bool on){
	if(expander){
		expander->pinWrite(BL_PIN, on ? LOW : HIGH);
	}else{
		digitalWrite(BL_PIN, on ? LOW : HIGH);
	}
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
	setBacklight(false);
	LED.setRGB(OFF);
	Playback.stop();
	delay(100);
	digitalWrite(SPEAKER_SD, HIGH);
#ifndef BYTEBOI_LAUNCHER
	btStop();
	esp_bt_controller_disable();
	esp_wifi_stop();
#endif

	gpio_hold_en((gpio_num_t) BL_PIN);
	gpio_hold_en((gpio_num_t) SPEAKER_SD);
	gpio_deep_sleep_hold_en();

	adc_power_off();
	esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
	esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
	esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
	esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
	esp_deep_sleep_start();

	for(;;); // Just in case
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

ByteBoiImpl::Ver ByteBoiImpl::getVer() const{
	return ver;
}
