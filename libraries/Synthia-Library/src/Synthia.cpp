#include "Synthia.h"
#include <WiFi.h>
#include <Devices/Matrix/MatrixOutput.h>
#include "Output/RGBExpanderOutput.h"
#include "PinDef.h"
#include <Devices/Matrix/MatrixOutputBuffer.h>
#include <Devices/Matrix/MatrixPartOutput.h>
#include <Util/HWRevision.h>
#include <Input/InputShift.h>

const i2s_pin_config_t i2s_pin_config = {
		.bck_io_num = I2S_BCK,
		.ws_io_num = I2S_WS,
		.data_out_num = I2S_DO,
		.data_in_num = I2S_DI
};

SynthiaImpl Synthia;
SliderInput Sliders;
EncoderInput Encoders;
PinMap<Pin> Pins;

SynthiaImpl::SynthiaImpl() : charlieBuffer(&charlie), trackOutput(&charlieBuffer), cursorOutput(&charlieBuffer), slidersOutput(&charlieBuffer),
							 TrackMatrix(trackOutput), CursorMatrix(cursorOutput), SlidersMatrix(slidersOutput),
							 RGBBuffer(10, 1), slotRGBOutput(&RGBBuffer), trackRGBOutput(&RGBBuffer),
							 TrackRGB(trackRGBOutput), SlotRGB(slotRGBOutput){

	CircuitOS::gd_set_old_transparency(true);
}

void SynthiaImpl::begin(){
	if(psramFound()){
		Serial.printf("PSRAM init: %s, free: %d B\n", psramInit() ? "Yes" : "No", ESP.getFreePsram());
	}else{
		Serial.println("No PSRAM detected");
	}

	disableCore0WDT();
	disableCore1WDT();

	initVer();

	btnToSlotMap = {
			{ BTN_1, 0 },
			{ BTN_2, 1 },
			{ BTN_3, 2 },
			{ BTN_4, 3 },
			{ BTN_5, 4 },
	};

	slotToBtnMap = {
			{ 0, BTN_1 },
			{ 1, BTN_2 },
			{ 2, BTN_3 },
			{ 3, BTN_4 },
			{ 4, BTN_5 },
	};

	analogReadResolution(10);
	analogSetAttenuation(ADC_11db);

	WiFi.mode(WIFI_OFF);
	btStop();

	Settings.begin();
	Sliders.begin();
	Encoders.begin();

	Wire.begin(Pins.get(Pin::I2C_Sda), Pins.get(Pin::I2C_Scl), 400000);

	if(ver == Ver::v2){
		Wire1.begin(Pins.get(Pin::I2C2_Sda), Pins.get(Pin::I2C2_Scl), 400000);

		inputExpander = new I2cExpander();
		inputExpander->begin(0b0100000, Wire1);

		input = new InputI2C(inputExpander);
	}else{
		auto* shift = new InputShift(Pins.get(Pin::ShiftData), Pins.get(Pin::ShiftClk), Pins.get(Pin::ShiftPl), 7);
		shift->begin();
		input = shift;
	}

	input->preregisterButtons({ BTN_1, BTN_2, BTN_3, BTN_4, BTN_5, BTN_ENC_L, BTN_ENC_R });
	LoopManager::addListener(input);


	if(!SPIFFS.begin()){
		Serial.println("couldn't start SPIFFS");
		for(;;);
	}


	charlie.init();
	const auto brightness = map(Settings.get().brightness, 0, 255, 50, 150);
	charlie.setBrightness(brightness);


	TrackMatrix.begin();
	CursorMatrix.begin();
	SlidersMatrix.begin();

	if(ver == Ver::v2){
		aw9523Track = new AW9523(Wire, 0x5B);
		aw9523Slot = new AW9523(Wire1, 0x5B);

		aw9523Track->begin();
		aw9523Slot->begin();

		aw9523Slot->setCurrentLimit(AW9523::IMAX_1Q);
		aw9523Track->setCurrentLimit(AW9523::IMAX_1Q);

		rgbExpOut = new RGBExpanderOutput();
		rgbExpOut->set(aw9523Slot, aw9523Track, {
				RGBExpanderOutput::PixelMapping{{ 0, (uint8_t) Pins.get(Pin::LED_R1) }, { 0, (uint8_t) Pins.get(Pin::LED_G1) }, { 0, (uint8_t) Pins.get(Pin::LED_B1) } },
				RGBExpanderOutput::PixelMapping{{ 0, (uint8_t) Pins.get(Pin::LED_R2) }, { 0, (uint8_t) Pins.get(Pin::LED_G2) }, { 0, (uint8_t) Pins.get(Pin::LED_B2) } },
				RGBExpanderOutput::PixelMapping{{ 0, (uint8_t) Pins.get(Pin::LED_R3) }, { 0, (uint8_t) Pins.get(Pin::LED_G3) }, { 0, (uint8_t) Pins.get(Pin::LED_B3) } },
				RGBExpanderOutput::PixelMapping{{ 0, (uint8_t) Pins.get(Pin::LED_R4) }, { 0, (uint8_t) Pins.get(Pin::LED_G4) }, { 0, (uint8_t) Pins.get(Pin::LED_B4) } },
				RGBExpanderOutput::PixelMapping{{ 0, (uint8_t) Pins.get(Pin::LED_R5) }, { 0, (uint8_t) Pins.get(Pin::LED_G5) }, { 0, (uint8_t) Pins.get(Pin::LED_B5) } },
				RGBExpanderOutput::PixelMapping{{ 1, (uint8_t) Pins.get(Pin::LED_R6) }, { 1, (uint8_t) Pins.get(Pin::LED_G6) }, { 1, (uint8_t) Pins.get(Pin::LED_B6) } },
				RGBExpanderOutput::PixelMapping{{ 1, (uint8_t) Pins.get(Pin::LED_R7) }, { 1, (uint8_t) Pins.get(Pin::LED_G7) }, { 1, (uint8_t) Pins.get(Pin::LED_B7) } },
				RGBExpanderOutput::PixelMapping{{ 1, (uint8_t) Pins.get(Pin::LED_R8) }, { 1, (uint8_t) Pins.get(Pin::LED_G8) }, { 1, (uint8_t) Pins.get(Pin::LED_B8) } },
				RGBExpanderOutput::PixelMapping{{ 1, (uint8_t) Pins.get(Pin::LED_R9) }, { 1, (uint8_t) Pins.get(Pin::LED_G9) }, { 1, (uint8_t) Pins.get(Pin::LED_B9) } },
				RGBExpanderOutput::PixelMapping{{ 1, (uint8_t) Pins.get(Pin::LED_R10) }, { 1, (uint8_t) Pins.get(Pin::LED_G10) }, { 1, (uint8_t) Pins.get(Pin::LED_B10) } }
		});
		rgbExpOut->init();

		RGBBuffer.setOutput(rgbExpOut);
	}else{
		rgbShift = new ShiftOutput(Pins.get(Pin::RGB_Clk), { (uint8_t) Pins.get(Pin::RGB_D1), (uint8_t) Pins.get(Pin::RGB_D2), (uint8_t) Pins.get(Pin::RGB_D3), (uint8_t) Pins.get(Pin::RGB_D4) });
		rgbShift->begin();

		rgbShiftOut = new RGBShiftOutput();
		rgbShiftOut->set(rgbShift, {
				RGBShiftOutput::PixelMapping{ { 0, 2 }, { 0, 1 }, { 0, 3 } },
				RGBShiftOutput::PixelMapping{ { 0, 7 }, { 0, 6 }, { 0, 0 } },
				RGBShiftOutput::PixelMapping{ { 0, 4 }, { 1, 0 }, { 0, 5 } },
				RGBShiftOutput::PixelMapping{ { 1, 2 }, { 1, 3 }, { 1, 1 } },
				RGBShiftOutput::PixelMapping{ { 1, 5 }, { 1, 6 }, { 1, 4 } },
				RGBShiftOutput::PixelMapping{ { 2, 5 }, { 2, 4 }, { 2, 6 } },
				RGBShiftOutput::PixelMapping{ { 2, 0 }, { 2, 7 }, { 2, 1 } },
				RGBShiftOutput::PixelMapping{ { 2, 3 }, { 2, 2 }, { 3, 4 } },
				RGBShiftOutput::PixelMapping{ { 3, 6 }, { 3, 5 }, { 3, 0 } },
				RGBShiftOutput::PixelMapping{ { 3, 2 }, { 3, 1 }, { 3, 3 } },
		});
		rgbShiftOut->init();

		RGBBuffer.setOutput(rgbShiftOut);
	}

	TrackRGB.begin();
	SlotRGB.begin();

	if(ver == Ver::v2){
		SlotRGB.setBrightness(Settings.get().brightness);
		TrackRGB.setBrightness(Settings.get().brightness);
	}
}

void SynthiaImpl::initVer(int override){
	if(verInited) return;
	verInited = true;

	const auto hw = override == -1 ? HWRevision::get() : override;

	if(hw == 1){
		ver = Ver::v2;
		Pins.set(Pins2);
	}else{
		ver = Ver::v1;
		Pins.set(Pins1);
	}
}

Input* SynthiaImpl::getInput() const{
	return input;
}


int SynthiaImpl::btnToSlot(uint8_t i){
	auto pair = btnToSlotMap.find(i);
	if(pair == btnToSlotMap.end()) return -1;
	return pair->second;
}

int SynthiaImpl::slotToBtn(uint8_t i){
	auto pair = slotToBtnMap.find(i);
	if(pair == slotToBtnMap.end()) return -1;
	return pair->second;
}

void SynthiaImpl::clearMatrices(){
	for(Matrix* matrix : { &TrackMatrix, &SlidersMatrix, &CursorMatrix, &SlotRGB, &TrackRGB }){
		matrix->stopAnimations();
		matrix->clear();
		matrix->push();
	}
}

IS31FL3731& SynthiaImpl::getCharlie(){
	return charlie;
}
