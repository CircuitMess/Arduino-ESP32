#ifndef ARMSTRONG_LIBRARY_COLORSENSOR_H
#define ARMSTRONG_LIBRARY_COLORSENSOR_H

#include <Loop/LoopListener.h>
#include <Display/Color.h>
#include <veml6040.h>

class ColorSensor : private LoopListener {
public:
	void begin();
	void end();

	void scan();

	Pixel getColor() const;
	uint8_t getR() const;
	uint8_t getG() const;
	uint8_t getB() const;

private:
	void loop(uint micros) override;

	VEML6040 sensor;
	Pixel color = { 0 };

	uint32_t readTimer = 0;
	static constexpr uint32_t ReadInterval = 350000; // [us]

};


#endif //ARMSTRONG_LIBRARY_COLORSENSOR_H
