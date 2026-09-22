#include "JayD.h"
#include <Devices/Matrix/MatrixOutputBuffer.h>
#include <Util/HWRevision.h>
#include <Util/gifdec.h>
#include <SD.h>
#include <SD_MMC.h>
#include "Pins.h"

IS31FL3731 charlie;
Matrix LEDmatrix(charlie);
MatrixOutputBuffer charlieBuffer(&charlie);
MatrixManager matrixManager(&charlieBuffer);

JayDImpl JayD;

JayDImpl::JayDImpl() : display(160, 128, -1, -1){}

void JayDImpl::begin(){
	if(psramFound()){
		Serial.printf("PSRAM init: %s, free: %d B\n", psramInit() ? "Yes" : "No", ESP.getFreePsram());
	} else{
		Serial.println("No PSRAM detected");
	}

	i2s_pin_config = {
		.bck_io_num = PIN(I2S_BCK),
		.ws_io_num = PIN(I2S_WS),
		.data_out_num = PIN(I2S_DO),
		.data_in_num = PIN(I2S_DI)
	};

	disableCore0WDT();
	disableCore1WDT();

	WiFi.mode(WIFI_OFF);
	btStop();

	CircuitOS::gd_set_old_transparency(true);

	initVer();

	//Ovo je neki SD pin na kojem ne smijemo imati pull-up, jer je strapping pin. Ali SD MMC ne radi bez ovoga, zato koristimo interni pull-up.
	if(ver == Ver::v1_3){
		gpio_set_pull_mode(GPIO_NUM_2, GPIO_PULLUP_ONLY);
	}
	SPI.begin(PIN(SPI_SCK), PIN(SPI_MISO), PIN(SPI_MOSI), PIN(SPI_SS));

	auto sdCheck = [](const Ver ver) {
		if(ver == Ver::v1_3){
			return !SD_MMC.begin("/sdcard", true);
		}
		SPI.setFrequency(60000000);
		return !SD.begin(PIN(SD_CS), SPI);
	};
	if(sdCheck(ver)){
		Serial.println("No SD card");
	}

	if(!SPIFFS.begin()){
		Serial.println("SPIFFS error");
	}

	if(ver == Ver::v1_3 || ver == Ver::v1_2){
		display.getTft()->setPanel(JayDDisplay::panel3());
	} else if(ver == Ver::v1_1){
		display.getTft()->setPanel(JayDDisplay::panel2());
	} else{
		display.getTft()->setPanel(JayDDisplay::panel1());
	}

	display.begin();
	SPI.setFrequency(20000000);

	Wire.begin(PIN(I2C_SDA), PIN(I2C_SCL));
	Wire.setClock(400000);

	charlie.init();
	LEDmatrix.begin();

	LoopManager::addListener(&Sched);
	LoopManager::addListener(&matrixManager);
	LoopManager::addListener(new InputJayD());
	InputJayD::getInstance()->begin();

	Settings.begin();
	LEDmatrix.setBrightness(80.0f * (float)Settings.get().brightnessLevel / 255.0f);
}

void JayDImpl::initVer(int override){
	if(verInited) return;
	verInited = true;

	static constexpr Ver map[] = { Ver::v1_0, Ver::v1_1, Ver::v1_2, Ver::v1_3 };
	const int hw = override == -1 ? HWRevision::get() : override;

	if(hw >= 0 && hw < sizeof(map) / sizeof(map[0])){
		ver = map[hw];
		Pins::setRev(hw);
	} else{
		verInited = false;
	}
}

Display& JayDImpl::getDisplay(){
	return display;
}

File JayDImpl::SD_open(const char* path, const char* mode){
	if(ver == Ver::v1_3){
		return SD_MMC.open(path, mode);
	}
	return SD.open(path, mode);
}

File JayDImpl::SD_open(String path, const char* mode){
	if(ver == Ver::v1_3){
		return SD_MMC.open(path, mode);
	}
	return SD.open(path, mode);
}

bool JayDImpl::SD_exists(const char* path){
	if(ver == Ver::v1_3){
		return SD_MMC.exists(path);
	}
	return SD.exists(path);
}

bool JayDImpl::SD_exists(const String& path){
	if(ver == Ver::v1_3){
		return SD_MMC.exists(path);
	}
	return SD.exists(path);
}

bool JayDImpl::SD_remove(const char* path){
	if(ver == Ver::v1_3){
		return SD_MMC.remove(path);
	}
	return SD.remove(path);
}

bool JayDImpl::SD_remove(const String& path){
	if(ver == Ver::v1_3){
		return SD_MMC.remove(path);
	}
	return SD.remove(path);
}

bool JayDImpl::SD_begin(){
	if(ver == Ver::v1_3){
		return SD_MMC.begin("/sdcard", true);
	}
	return SD.begin(22, SPI);
}

void JayDImpl::SD_end(){
	if(ver == Ver::v1_3){
		return SD_MMC.end();
	}
	return SD.end();
}
