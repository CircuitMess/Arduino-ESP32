#include "JayD.h"
#include <Devices/Matrix/MatrixOutputBuffer.h>
#include <Util/HWRevision.h>
#include <Util/gifdec.h>

const i2s_pin_config_t i2s_pin_config = {
		.bck_io_num = I2S_BCK,
		.ws_io_num = I2S_WS,
		.data_out_num = I2S_DO,
		.data_in_num = I2S_DI
};

IS31FL3731 charlie;
Matrix LEDmatrix(charlie);
MatrixOutputBuffer charlieBuffer(&charlie);
MatrixManager matrixManager(&charlieBuffer);

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

	CircuitOS::gd_set_old_transparency(true);

	initVer();

	SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SPI_SS);
	SPI.setFrequency(60000000);
	if(!SD.begin(SD_CS, SPI)){
		Serial.println("No SD card");
	}
	if(!SPIFFS.begin()){
		Serial.println("SPIFFS error");
	}

	if(HWRevision::get() == 2){
		display.getTft()->setPanel(JayDDisplay::panel3());
	}else if(HWRevision::get() == 1){
		display.getTft()->setPanel(JayDDisplay::panel2());
	}else{
		display.getTft()->setPanel(JayDDisplay::panel1());
	}
	display.begin();
	SPI.setFrequency(20000000);

	Wire.begin(I2C_SDA, I2C_SCL);
	Wire.setClock(400000);

	charlie.init();
	LEDmatrix.begin();

	LoopManager::addListener(&Sched);
	LoopManager::addListener(&matrixManager);
	LoopManager::addListener(new InputJayD());
	InputJayD::getInstance()->begin();

	Settings.begin();
	LEDmatrix.setBrightness(80.0f * (float) Settings.get().brightnessLevel / 255.0f);
}

void JayDImpl::initVer(int override){
	if(verInited) return;
	verInited = true;

	static constexpr Ver map[] = { Ver::v1_0, Ver::v1_1, Ver::v1_2 };
	const auto hw = override == -1 ? HWRevision::get() : override;

	if(hw >= 0 && hw < sizeof(map) / sizeof(map[0])){
		ver = map[hw];
	}else{
		verInited = false;
	}
}

Display& JayDImpl::getDisplay(){
	return display;
}
