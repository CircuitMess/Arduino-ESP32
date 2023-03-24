#ifndef BATMOBILE_FIRMWARE_LEDCONTROLLER_H
#define BATMOBILE_FIRMWARE_LEDCONTROLLER_H

#include <Loop/LoopListener.h>
#include <glm.h>
#include <Arduino.h>
#include <vector>

template<typename T>
class LEDController : public LoopListener {
public:
	void begin();
	void end();

	void clear();
	void setSolid(T color);
	void blink(T color);
	void blinkTwice(T color);
	/**
	 * Starts a continuous blinking in the selected color.
	 * @param color Selected color
	 * @param duration Duration of LED on/off cycle. LED will stay the same color for 'duration' ms, then turn off for 'duration' ms.
	 */
	void blinkContinuous(T color, uint32_t duration = 500);

	/**
	 * Breathes an LED from specified start value to end value.
	 * Typename T needs to have defined subtraction (T start - T end) and multiplication by a scalar value (T start * float n)
	 * @param start Starting value
	 * @param end Ending value
	 * @param period Duration of 1 period, going from start to end and back, in milliseconds.
	 * @param loops Number of loops, -1 for infinite looping.
	 */
	void breathe(T start, T end, size_t period, int16_t loops = -1);
	void loop(uint micros) final;

protected:
	virtual void init() = 0;
	virtual void deinit() = 0;
	virtual void write(T val) = 0;

private:
	enum {
		Solid, Once, Twice, Continuous, Breathe
	} LEDstate = Solid;

	T LEDcolor{};

	T blinkColor{};
	uint32_t blinkStartTime = 0;
	bool blinkState = false;
	uint32_t blinkContinuousDuration = 0; //[ms]

	bool breatheQueued = false; // used to prevent breathe command from interrupting blink/doubleBlink
	T breatheStart{};
	T breatheEnd{};
	size_t breathePeriod;
	size_t breatheMillis;
	int16_t breatheLoops;
	uint16_t breatheLoopCounter;

	static constexpr uint32_t blinkDuration = 200; //[ms]

	void setValue(T color);
};


class SingleLEDController : public LEDController<uint8_t> {
public:
	SingleLEDController(uint8_t pin, uint8_t channel);

protected:
	void write(uint8_t val) override;

	void init() override;
	void deinit() override;

private:
	const uint8_t pin;
	const uint8_t channel;
};

//RGBLEDController template type is vector of floats to allow multiplication by a float value, this is a limitation set by glm
class RGBLEDController : public LEDController<glm::vec3> {
public:
	RGBLEDController(std::initializer_list<uint8_t> pins, std::initializer_list<uint8_t> channels);

protected:
	void write(glm::vec3 val) override;

	void init() override;
	void deinit() override;

private:
	const std::vector<uint8_t> pins;
	const std::vector<uint8_t> channels;
};


#endif //BATMOBILE_FIRMWARE_LEDCONTROLLER_H
