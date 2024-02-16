#include <Arduino.h>
#include <CircuitOS.h>
#include "src/Clockstar.h"
#include "src/Battery/BatteryService.h"
#include <Loop/LoopManager.h>
#include <unordered_map>
Display* display;

std::unordered_map<int, const char*> btnMap = {
		{ BTN_SELECT, "Select" },
		{ BTN_DOWN,  "Down" },
		{ BTN_UP, "Up" },
		{ BTN_B,     "Alt" }
};

class InputTest : public InputListener, public LoopListener {
public:
	InputTest(){
		LoopManager::addListener(this);
		Input::getInstance()->addListener(this);

		pinMode(PIN_CHARGE, INPUT);
		imu = Clockstar.getIMU();
	}

private:

	LSM6DS3* imu;
	float ax = 0;
	float ay = 0;
	float az = 0;

	void buttonPressed(uint i) override{
		button = i;
	}

	void buttonReleased(uint i) override{
		button = -1;
	}

	void draw(){
		auto btn = btnMap.find(button);

		Sprite* canvas = display->getBaseSprite();
		canvas->clear(TFT_RED);

		if(btn != btnMap.end()){
			canvas->setCursor(50, 90);
			canvas->print(btn->second);
		}

		canvas->setCursor(4, 105);
		canvas->printf("%.1f, %.1f, %.1f", ax, ay, az);

		display->commit();
	}

	void loop(uint micros) override{

		ax = imu->readFloatAccelX();
		ay = imu->readFloatAccelY();
		az = imu->readFloatAccelZ();

		draw();
	}

	int button = -1;
};

void setup(){
	Clockstar.begin();
	display = Clockstar.getDisplay();

	RGB.setSolid(Pixel::Red);
	new InputTest();
}

void loop(){
	LoopManager::loop();
//	Clockstar.getDisplay()->clear(TFT_RED);
//	const auto sprite = Clockstar.getDisplay()->getBaseSprite();
//
//	sprite->setCursor(0,0);
//	sprite->printf("bat: %d, charge: %d\n", Battery.getPercentage(), Battery.charging());
//	Clockstar.getDisplay()->commit();
//	delay(100);
}