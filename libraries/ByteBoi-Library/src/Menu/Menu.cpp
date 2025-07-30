#include "Menu.h"
#include "../ByteBoi.h"
#include <Loop/LoopManager.h>
#include "../Settings.h"
#include <SPIFFS.h>
#include <FS/CompressedFile.h>
#include "../Playback/PlaybackSystem.h"

MiniMenu::Menu* MiniMenu::Menu::instance = nullptr;

MiniMenu::Menu::Menu(Context* currentContext) : Modal(*currentContext, 130, ByteBoi.isStandalone() ? 56 : 72), canvas(screen.getSprite()){

	instance = this;

	volumeSlider = new SliderElement(&screen, "Volume", [](int value){
		Settings.get().volume = value;
		Playback.updateGain();
	});
	volumeSlider->setSliderValue(Settings.get().volume);
	elements[0] = volumeSlider;

	const auto version = ByteBoi.getVer();
	if(version == ByteBoiImpl::v2_0){
		LEDSwitch = nullptr;
		NumElements = 1;
	}else{
		LEDSwitch = new Switch(&screen, "LEDs", [](int value){
			Settings.get().RGBenable = value;
			if(!value){
				LED.setRGB(OFF);
			}else{
				LED.setRGB(LEDColor::WHITE);
				instance->blinkTime = millis();
				LoopManager::addListener(instance);
			}
		});
		LEDSwitch->set(Settings.get().RGBenable, true);
		elements[1] = LEDSwitch;
		NumElements = 2;
	}


	// TODO: rework this check, exit should appear if this is the game partition
	if(!ByteBoi.isStandalone()){
		exit = new TextElement(&screen, "Exit game", [](int value){
			instance->stop();
			Playback.stop();
			ByteBoi.backToLauncher();
		});
		elements[NumElements] = exit;
		NumElements++;
	}else{
		exit = nullptr;
	}

	buildUI();
}

MiniMenu::Menu::~Menu(){
	for(uint8_t i = 0; i < NumElements; i++){
		delete elements[i];
	}
}

void MiniMenu::Menu::start(){
	selectedElement = 0;
	volumeSlider->setIsSelected(true);
	bindInput();
	if(LEDSwitch != nullptr){
		LEDSwitch->set(Settings.get().RGBenable, true);
	}
	volumeSlider->setSliderValue(Settings.get().volume);
	LoopManager::addListener(this);
}

void MiniMenu::Menu::stop(){
	Settings.get().volume = instance->volumeSlider->getSliderValue();
	if(LEDSwitch != nullptr){
		Settings.get().RGBenable = instance->LEDSwitch->getState();
		LED.setRGB(LEDColor::OFF);
	}
	Playback.updateGain();

	bool playing = Playback.isRunning();
	Playback.stop();
	delay(50);
	Settings.store();
	if(playing){
		Playback.start();
	}

	releaseInput();
	LoopManager::removeListener(this);
}

void MiniMenu::Menu::bindInput(){
	Input::getInstance()->setBtnPressCallback(BTN_B, [](){
		if(instance == nullptr) return;
		instance->pop();
	});

	Input::getInstance()->setBtnPressCallback(BTN_A, [](){
		if(instance == nullptr) return;
		instance->elements[instance->selectedElement]->click();

		Playback.tone(500, 100);
	});

	Input::getInstance()->setBtnPressCallback(BTN_UP, [](){
		if(instance == nullptr) return;
		if(instance->NumElements == 1) return;

		instance->elements[instance->selectedElement]->setIsSelected(false);
		instance->selectedElement--;
		if(instance->selectedElement < 0){
			instance->selectedElement = instance->NumElements - 1;
		}
		instance->elements[instance->selectedElement]->setIsSelected(true);
		Playback.tone(500, 50);
	});

	Input::getInstance()->setBtnPressCallback(BTN_DOWN, [](){
		if(instance == nullptr) return;
		if(instance->NumElements == 1) return;

		instance->elements[instance->selectedElement]->setIsSelected(false);
		instance->selectedElement++;
		if(instance->selectedElement >= instance->NumElements){
			instance->selectedElement = 0;
		}
		instance->elements[instance->selectedElement]->setIsSelected(true);
		Playback.tone(500, 50);
	});

	Input::getInstance()->setBtnPressCallback(BTN_RIGHT, [](){
		if(instance == nullptr) return;

		instance->elements[instance->selectedElement]->right();

		Playback.tone(500, 100);
	});

	Input::getInstance()->setBtnPressCallback(BTN_LEFT, [](){
		if(instance == nullptr) return;

		instance->elements[instance->selectedElement]->left();

		Playback.tone(500, 100);
	});

	Input::getInstance()->setBtnPressCallback(BTN_C, [](){
		if(instance == nullptr) return;
		instance->pop();
	});

	Input::getInstance()->setButtonHeldRepeatCallback(BTN_RIGHT, 200, [](uint){
		if(instance == nullptr || instance->selectedElement != 0) return;

		instance->volumeSlider->right();

		Playback.tone(500, 100);

		instance->draw();
		instance->screen.commit();
	});
	Input::getInstance()->setButtonHeldRepeatCallback(BTN_LEFT, 200, [](uint){
		if(instance == nullptr || instance->selectedElement != 0) return;

		instance->volumeSlider->left();

		Playback.tone(500, 100);

		instance->draw();
		instance->screen.commit();
	});

}

void MiniMenu::Menu::releaseInput(){
	Input::getInstance()->removeBtnPressCallback(BTN_UP);
	Input::getInstance()->removeBtnPressCallback(BTN_DOWN);
	Input::getInstance()->removeBtnPressCallback(BTN_LEFT);
	Input::getInstance()->removeBtnPressCallback(BTN_RIGHT);
	Input::getInstance()->removeBtnPressCallback(BTN_A);
	Input::getInstance()->removeBtnPressCallback(BTN_B);
	Input::getInstance()->removeBtnPressCallback(BTN_C);
	Input::getInstance()->removeButtonHeldRepeatCallback(BTN_LEFT);
	Input::getInstance()->removeButtonHeldRepeatCallback(BTN_RIGHT);
}

void MiniMenu::Menu::draw(){
	canvas->clear(TFT_TRANSPARENT);
	canvas->fillRoundRect(screen.getTotalX(), screen.getTotalY(), canvas->width(), canvas->height(), 3, C_HEX(0x004194));
	canvas->fillRoundRect(screen.getTotalX() + 2, screen.getTotalY() + 2, canvas->width() - 4, canvas->height() - 4, 3, C_HEX(0x0041ff));
	Battery.drawIcon(*canvas, 111, 4);

	for(uint8_t i = 0; i < NumElements; i++){
		elements[i]->draw();
	}
	screen.draw();
}

void MiniMenu::Menu::loop(uint micros){
	if(blinkTime != 0 && millis() - blinkTime >= 200){
		blinkTime = 0;
		LED.setRGB(LEDColor::OFF);
	}

	elements[selectedElement]->loop(micros);

	draw();
	screen.commit();
}

void MiniMenu::Menu::buildUI(){
	int x = 7;
	//All elements are SettingsElements of height 20
	int y = (canvas->height() - NumElements * 20) / 2;

	for(uint8_t i = 0; i < NumElements; i++){
		elements[i]->setPos(x, y);
		y += elements[i]->getHeight();
	}

	//Center the "Exit Game" text
	if(exit != nullptr){
		exit->setX((canvas->width() - exit->getWidth()) / 2);
	}
}

void MiniMenu::Menu::returned(void* data){
	prevModal = (Modal*) data;
}

void MiniMenu::Menu::popIntoPrevious(){
	ModalTransition* transition = static_cast<ModalTransition*>((void*) instance->pop());
	if(instance->prevModal == nullptr) return;
	transition->setDoneCallback([](Context* currContext, Modal*){
		instance->prevModal->push(currContext);
	});
//		instance->pop();
}
