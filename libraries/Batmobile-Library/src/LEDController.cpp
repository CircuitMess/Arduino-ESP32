#include "LEDController.h"
#include <Loop/LoopManager.h>

static constexpr uint PWMFreq = 5000;
static constexpr uint8_t PWMResolution = 8;


template<typename T>
void LEDController<T>::begin(){
	init();
	clear();
	LoopManager::addListener(this);
}

template<typename T>
void LEDController<T>::end(){
	LoopManager::removeListener(this);
	clear();
	deinit();
}

template<typename T>
void LEDController<T>::setValue(T color){
	LEDcolor = color;

	if(LEDstate == Solid){
		write(color);
	}
}

template<typename T>
void LEDController<T>::clear(){
	LEDstate = Solid;
	blinkState = false;
	blinkColor = T();
	blinkStartTime = 0;
	LEDcolor = T();
	breatheQueued = false;

	write(LEDcolor);
}

template<typename T>
void LEDController<T>::setSolid(T color){
	LEDstate = Solid;
	setValue(color);
}

template<typename T>
void LEDController<T>::blink(T color){
	LEDstate = Once;
	blinkColor = color;
	blinkStartTime = millis();
	blinkState = true;

	write(color);
}

template<typename T>
void LEDController<T>::blinkTwice(T color){
	blink(color);
	LEDstate = Twice;
}

template<typename T>
void LEDController<T>::blinkContinuous(T color, uint32_t duration){
	blink(color);
	blinkContinuousDuration = duration;
	LEDstate = Continuous;
}

template<typename T>
void LEDController<T>::breathe(T start, T end, size_t period, int16_t loops){
	if(loops == 0) return;

	breathePeriod = period;
	breatheLoops = loops;
	breatheStart = start;
	breatheEnd = end;
	breatheLoopCounter = 0;
	breatheMillis = millis();

	if(LEDstate == Once || LEDstate == Twice){
		//can't interrupt blinking with breathe command
		breatheQueued = true;
		return;
	}

	LEDstate = Breathe;
	write(breatheStart);
}

template<typename T>
void LEDController<T>::loop(uint micros){
	if(LEDstate == Solid) return;
	else if(LEDstate == Twice || LEDstate == Once){
		if(millis() - blinkStartTime < blinkDuration) return;
	}else if(LEDstate == Continuous){
		if(millis() - blinkStartTime < blinkContinuousDuration) return;
	}

	bool push = false;
	T pushVal{};

	if(LEDstate == Continuous){
		blinkState = !blinkState;
		blinkStartTime = millis();
		pushVal = blinkState ? blinkColor : T();
		push = true;
	}else if(LEDstate == Twice && blinkState){
		blinkState = false;
		blinkStartTime = millis();
		pushVal = T();
		push = true;
	}else if(LEDstate == Twice && !blinkState){
		blinkState = true;
		blinkStartTime = millis();
		LEDstate = Once;
		pushVal = blinkColor;
		push = true;
	}else if(LEDstate == Once){
		blinkState = false;
		blinkStartTime = 0;
		blinkColor = T();

		if(breatheQueued){
			LEDstate = Breathe;
		}else{
			LEDstate = Solid;
		}

		pushVal = LEDcolor;
		push = true;
	}else if(LEDstate == Breathe){
		if(millis() - breatheMillis >= breathePeriod){
			breatheMillis = millis();
			if(breatheLoops != -1){
				breatheLoopCounter++;
				if(breatheLoopCounter >= breatheLoops){
					LEDstate = Solid;
					pushVal = LEDcolor;
					write(pushVal);
					return;
				}
			}
		}
		push = true;

		float t = 0.5 * cos(2 * PI * (millis() - breatheMillis) / breathePeriod) + 0.5;

		T startPart = breatheStart;
		startPart *= t;
		T endPart = breatheEnd;
		endPart *= (1.0f - t);
		pushVal = startPart + endPart;

	}

	if(push){
		write(pushVal);
	}
}


template class LEDController<uint8_t>;
SingleLEDController::SingleLEDController(uint8_t pin, uint8_t channel) : pin(pin), channel(channel){ }

void SingleLEDController::init(){
	ledcSetup(channel, PWMFreq, PWMResolution);
	ledcWrite(channel, 0);
	ledcAttachPin(pin, channel);
}

void SingleLEDController::deinit(){
	ledcDetachPin(pin);
	pinMode(pin, OUTPUT);
	digitalWrite(pin, LOW);
}

void SingleLEDController::write(uint8_t val){
	ledcWrite(channel, val);
}


template class LEDController<glm::vec3>;
RGBLEDController::RGBLEDController(std::initializer_list<uint8_t> pins, std::initializer_list<uint8_t> channels) : pins(pins), channels(channels){ }

void RGBLEDController::init(){
	for(uint8_t i = 0; i < 3; i++){
		ledcSetup(channels[i], PWMFreq, PWMResolution);
		ledcWrite(channels[i], 0);
		ledcAttachPin(pins[i], channels[i]);
	}
}

void RGBLEDController::deinit(){
	for(uint8_t i = 0; i < 3; i++){
		ledcDetachPin(pins[i]);
		pinMode(pins[i], OUTPUT);
		digitalWrite(pins[i], LOW);
	}
}

void RGBLEDController::write(glm::vec3 val){
	for(uint8_t i = 0; i < 3; i++){
		ledcWrite(channels[i], val[i]);
	}
}
