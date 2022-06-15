#include "Synthia.h"
#include <WiFi.h>
#include <Devices/Matrix/MatrixOutput.h>
#include "Output/RGBMatrixOutput.h"
#include <Devices/Matrix/MatrixOutputBuffer.h>
#include <Devices/Matrix/MatrixPartOutput.h>
#include <unordered_map>

const i2s_pin_config_t i2s_pin_config = {
		.bck_io_num = I2S_BCK,
		.ws_io_num = I2S_WS,
		.data_out_num = I2S_DO,
		.data_in_num = I2S_DI
};

SynthiaImpl Synthia;
SliderInput Sliders;
EncoderInput Encoders;

SynthiaImpl::SynthiaImpl() : charlieBuffer(&charlie), trackOutput(&charlieBuffer), cursorOutput(&charlieBuffer), slidersOutput(&charlieBuffer),
							 TrackMatrix(trackOutput), CursorMatrix(cursorOutput), SlidersMatrix(slidersOutput),
							 RGBShiftOutput(RGB_CLK, RGB_DATA), RGBBuffer(&RGBOutput), slotRGBOutput(&RGBBuffer), trackRGBOutput(&RGBBuffer),
							 TrackRGB(trackRGBOutput), SlotRGB(slotRGBOutput){

}

void SynthiaImpl::begin(){
	if(psramFound()){
		Serial.printf("PSRAM init: %s, free: %d B\n", psramInit() ? "Yes" : "No", ESP.getFreePsram());
	}else{
		Serial.println("No PSRAM detected");
	}

	disableCore0WDT();
	disableCore1WDT();

	analogReadResolution(10);
	analogSetAttenuation(ADC_11db);

	WiFi.mode(WIFI_OFF);
	btStop();

	Settings.begin();
	Sliders.begin();
	Encoders.begin();

	input = new InputShift(INP_MISO, INP_CLK, INP_PL, 7);
	input->begin();
	input->preregisterButtons({BTN_1, BTN_2, BTN_3, BTN_4, BTN_5, BTN_ENC_L, BTN_ENC_R});
	LoopManager::addListener(input);

	if(!SPIFFS.begin()){
		Serial.println("couldn't start SPIFFS");
		for(;;);
	}


	Wire.begin(I2C_SDA, I2C_SCL);
	Wire.setClock(400000);

	charlie.init();
	charlie.setBrightness(Settings.get().brightness);

	TrackMatrix.begin();
	CursorMatrix.begin();
	SlidersMatrix.begin();

	RGBShiftOutput.begin();

	RGBOutput.set(&RGBShiftOutput, {
		RGBMatrixOutput::PixelMapping{ { 0, 2 }, { 0, 1 }, { 0, 3 } },
		RGBMatrixOutput::PixelMapping{ { 0, 7 }, { 0, 6 }, { 0, 0 } },
		RGBMatrixOutput::PixelMapping{ { 0, 4 }, { 1, 0 }, { 0, 5 } },
		RGBMatrixOutput::PixelMapping{ { 1, 2 }, { 1, 3 }, { 1, 1 } },
		RGBMatrixOutput::PixelMapping{ { 1, 5 }, { 1, 6 }, { 1, 4 } },
		RGBMatrixOutput::PixelMapping{ { 2, 5 }, { 2, 4 }, { 2, 6 } },
		RGBMatrixOutput::PixelMapping{ { 2, 0 }, { 2, 7 }, { 2, 1 } },
		RGBMatrixOutput::PixelMapping{ { 2, 3 }, { 2, 2 }, { 3, 4 } },
		RGBMatrixOutput::PixelMapping{ { 3, 6 }, { 3, 5 }, { 3, 0 } },
		RGBMatrixOutput::PixelMapping{ { 3, 2 }, { 3, 1 }, { 3, 3 } },
	});
	RGBOutput.init();

	TrackRGB.begin();
	SlotRGB.begin();
}

InputShift* SynthiaImpl::getInput() const{
	return input;
}


int SynthiaImpl::btnToSlot(uint8_t i){
	static const std::unordered_map<uint8_t, uint8_t> map = {
			{ BTN_1, 0 },
			{ BTN_2, 1 },
			{ BTN_3, 2 },
			{ BTN_4, 3 },
			{ BTN_5, 4 },
	};

	auto pair = map.find(i);
	if(pair == map.end()) return -1;
	return pair->second;
}

int SynthiaImpl::slotToBtn(uint8_t i){
	static const std::unordered_map<uint8_t, uint8_t> map = {
			{ 0, BTN_1 },
			{ 1, BTN_2 },
			{ 2, BTN_3 },
			{ 3, BTN_4 },
			{ 4, BTN_5 },
	};

	auto pair = map.find(i);
	if(pair == map.end()) return -1;
	return pair->second;
}

void SynthiaImpl::clearMatrices(){
	for(auto matrix : { Synthia.TrackMatrix, Synthia.SlidersMatrix, Synthia.CursorMatrix, Synthia.SlotRGB, Synthia.TrackRGB }){
		matrix.stopAnimations();
		matrix.clear();
		matrix.push();
	}
}

IS31FL3731& SynthiaImpl::getCharlie(){
	return charlie;
}
