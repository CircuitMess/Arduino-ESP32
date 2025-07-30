#ifndef SYNTHIA_LIBRARY_RGBSHIFTOUTPUT_H
#define SYNTHIA_LIBRARY_RGBSHIFTOUTPUT_H

#include <Arduino.h>
#include <Devices/ShiftOutput.h>
#include <Devices/Matrix/MatrixOutput.h>

// 10x1: 0-4 track matrix, 5-9 slot matrix
class RGBShiftOutput : public MatrixOutput {
public:
	struct PixelPin { uint8_t index, pin; };
	struct PixelMapping { PixelPin pinR, pinG, pinB; };

	RGBShiftOutput();

	void set(ShiftOutput* output, const std::array<PixelMapping, 10>& map);

	void init() override;
	void push(const MatrixPixelData& data) override;

private:
	ShiftOutput* output;
	std::array<PixelMapping, 10> map;

};


#endif //SYNTHIA_LIBRARY_RGBSHIFTOUTPUT_H
