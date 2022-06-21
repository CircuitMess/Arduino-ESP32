#ifndef SYNTHIA_LIBRARY_SYNTHIA_H
#define SYNTHIA_LIBRARY_SYNTHIA_H

#include <Arduino.h>
#include <CircuitOS.h>
#include <CMAudio.h>
#include <Loop/LoopListener.h>
#include <Loop/LoopManager.h>
#include <Input/InputShift.h>
#include <driver/i2s.h>
#include <Devices/AW9523.h>
#include "Input/SliderInput.h"
#include "Input/EncoderInput.h"
#include "Pins.hpp"
#include "Output/RGBMatrixOutput.h"
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

extern const i2s_pin_config_t i2s_pin_config;

class SynthiaImpl {
public:
	SynthiaImpl();

	void begin();

	void clearMatrices();

	InputShift* getInput() const;
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

	RGBMatrixOutput RGBOutput;
	MatrixOutputBuffer RGBBuffer;
	SlotRGBOutput slotRGBOutput;
	TrackRGBOutput trackRGBOutput;

	InputShift* input;
	ShiftOutput RGBShiftOutput;

public:
	Matrix TrackMatrix; //main 16x5 partition
	Matrix CursorMatrix; //16x1 strip below trackMatrix
	Matrix SlidersMatrix; //2x8 strip next to sliders
	Matrix TrackRGB; //2x5 RGB LEDs left and right from trackMatrix
	Matrix SlotRGB; //5 RGB LEDs below the slot buttons

};

extern SynthiaImpl Synthia;
extern SliderInput Sliders;
extern EncoderInput Encoders;

#endif
