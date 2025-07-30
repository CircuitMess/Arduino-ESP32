#ifndef SYNTHIA_LIBRARY_RGBEXPANDEROUTPUT_H
#define SYNTHIA_LIBRARY_RGBEXPANDEROUTPUT_H

#include <Arduino.h>
#include <Devices/Matrix/MatrixOutput.h>
#include <Devices/AW9523.h>
#include <array>

// 10x1: 0-4 track matrix, 5-9 slot matrix
class RGBExpanderOutput : public MatrixOutput {
public:
	struct PixelPin {
		uint8_t index; //Represents index of I2C expander, or shift register
		uint8_t pin; //Represents local pin of the specific indexed expander/shiftreg
	};
	struct PixelMapping {
		//Every RGB LED needs per-pin mapping for each channel
		PixelPin pinR, pinG, pinB;
	};

	RGBExpanderOutput();

	void set(AW9523* slotAW, AW9523* trackAW, const std::array<PixelMapping, 10>& map);

	void init() override;
	void push(const MatrixPixelData& data) override;

private:
	AW9523* slotAW;
	AW9523* trackAW;
	std::array<PixelMapping, 10> map;

};


#endif //SYNTHIA_LIBRARY_RGBEXPANDEROUTPUT_H
