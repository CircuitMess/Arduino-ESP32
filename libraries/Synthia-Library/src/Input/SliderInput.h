#ifndef SYNTHIA_LIBRARY_SLIDERINPUT_H
#define SYNTHIA_LIBRARY_SLIDERINPUT_H

#include <Loop/LoopListener.h>
#include <deque>
#include <queue>
#include <ResponsiveAnalogRead.h>
#include "../WithListeners.h"

class SliderInput;

class SliderListener {
friend SliderInput;

private:
	virtual void leftPotMove(uint8_t value);
	virtual void rightPotMove(uint8_t value);

};

template<typename T, size_t MaxLen, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
class FixedQueue : public std::deque<T> {
public:
	void push(const T& value){
		if(std::deque<T>::size() == MaxLen){
			std::deque<T>::pop_front();
		}
		std::deque<T>::push_back(value);
	}

	T getMedian(){
		size_t size = std::deque<T>::size();
		if(size == 0) return 0;

		std::vector<T> sorted(std::deque<T>::cbegin(), std::deque<T>::cend());
		std::sort(sorted.begin(), sorted.end());

		return sorted[size / 2];
	}

	T getAverage(){
		size_t size = std::deque<T>::size();
		T sum = 0;
		for(int i = 0; i < size; i++){
			sum += std::deque<T>::at(i);
		}
		return sum / size;
	}

	size_t getMax(){
		return MaxLen;
	}
};

class SliderInput : public LoopListener, public WithListeners<SliderListener> {
public:
	SliderInput();

	void begin();
	void loop(uint time) override;

	uint8_t getLeftPotValue() const;
	uint8_t getRightPotValue() const;

	void setLeftPotCallback(void (* leftPotCallback)(uint8_t));
	void setRightPotCallback(void (* rightPotCallback)(uint8_t));

private:
	uint8_t leftPotValue;
	uint8_t rightPotValue;

	int16_t leftPotPreviousValue = -1;
	int16_t rightPotPreviousValue = -1;

	uint16_t leftFilter(uint16_t sensorValue);
	uint16_t rightFilter(uint16_t sensorValue);

	const uint8_t MinPotMove = 1; // Minimum value change before listeners are triggered
	const float EMA_a = 0.01; // Filter alpha
	double rightEMA_S = 0;
	double leftEMA_S = 0;

	FixedQueue<uint32_t, 100> valsLeft;
	FixedQueue<uint32_t, 100> valsRight;

	ResponsiveAnalogRead respLeft;
	ResponsiveAnalogRead respRight;

	int16_t leftPotRawPreviousValue = -1;
	int16_t rightPotRawPreviousValue = -1;

	void (*leftPotCallback)(uint8_t value) = nullptr;
	void (*rightPotCallback)(uint8_t value) = nullptr;
};

#endif //SYNTHIA_LIBRARY_SLIDERINPUT_H