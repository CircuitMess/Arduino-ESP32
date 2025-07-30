#ifndef SYNTHIA_LIBRARY_SYNTHIA_H
#define SYNTHIA_LIBRARY_SYNTHIA_H

#include <Arduino.h>
#include <CircuitOS.h>
#include <CMAudio.h>
#include <Loop/LoopListener.h>
#include <Loop/LoopManager.h>
#include <Input/InputI2C.h>
#include <driver/i2s.h>
#include <Devices/AW9523.h>
#include "Input/SliderInput.h"
#include "Input/EncoderInput.h"
#include "Pins.hpp"
#include "Output/RGBExpanderOutput.h"
#include <Devices/Matrix/Matrix.h>
#include <Devices/Matrix/IS31FL3731.h>
#include "Output/CursorMatrixOutput.h"
#include "Output/SlidersMatrixOutput.h"
#include "Output/TrackMatrixOutput.h"
#include "Output/SlotRGBOutput.h"
#include "Output/TrackRGBOutput.h"
#include <Devices/Matrix/DelayedMatrixOutput.h>
#include <Devices/Matrix/MatrixAnimGIF.h>
#include <SPIFFS.h>
#include <Input/InputGPIO.h>
#include "Settings.h"
#include "Output/RGBShiftOutput.h"
#include <Input/I2cExpander.h>
#include <Util/PinMap.h>
#include <unordered_map>

extern const i2s_pin_config_t i2s_pin_config;

class SynthiaImpl {
public:
	SynthiaImpl();

	void begin();
	void initVer(int override = -1); // Initializes version and pins; also called from begin()

	void clearMatrices();

	Input* getInput() const;
	IS31FL3731& getCharlie();

	int btnToSlot(uint8_t i);
	int slotToBtn(uint8_t i);
private:
	//hardware outputting
	IS31FL3731 charlie;
	MatrixOutputBuffer charlieBuffer;
	TrackMatrixOutput trackOutput;
	CursorMatrixOutput cursorOutput;
	SlidersMatrixOutput slidersOutput;

	MatrixOutputBuffer RGBBuffer;
	SlotRGBOutput slotRGBOutput;
	TrackRGBOutput trackRGBOutput;

	Input* input;

	// HW v1
	ShiftOutput* rgbShift = nullptr;
	RGBShiftOutput* rgbShiftOut = nullptr;

	// HW v2
	I2cExpander* inputExpander = nullptr;
	AW9523* aw9523Track;
	AW9523* aw9523Slot;
	RGBExpanderOutput* rgbExpOut = nullptr;

	enum class Ver { v1, v2 } ver = Ver::v1;
	bool verInited = false;

public:
	Matrix TrackMatrix; //main 16x5 partition
	Matrix CursorMatrix; //16x1 strip below trackMatrix
	Matrix SlidersMatrix; //2x8 strip next to sliders
	Matrix TrackRGB; //2x5 RGB LEDs left and right from trackMatrix
	Matrix SlotRGB; //5 RGB LEDs below the slot buttons

private:
	std::unordered_map<uint8_t, uint8_t> btnToSlotMap;
	std::unordered_map<uint8_t, uint8_t> slotToBtnMap;

};

extern SynthiaImpl Synthia;
extern SliderInput Sliders;
extern EncoderInput Encoders;
extern PinMap<Pin> Pins;

#endif
