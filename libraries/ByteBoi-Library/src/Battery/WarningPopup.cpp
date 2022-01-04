#include "WarningPopup.h"
#include "../Bitmaps/low.hpp"
#include <Loop/LoopManager.h>
#include <Support/ModalTransition.h>
#include <SPIFFS.h>

const uint8_t WarningPopup::warningTime = 5;
WarningPopup* WarningPopup::instance = nullptr;

WarningPopup::WarningPopup(Context &context) : Modal(context, 135, 60){
	instance = this;
	screen.getSprite()->setChroma(TFT_TRANSPARENT);
	buffer= static_cast<Color*>(malloc(sizeof(low)));
	memcpy_P(buffer,low,sizeof(low));
}

WarningPopup::~WarningPopup(){
	free(buffer);
}

void WarningPopup::draw(){
	Sprite& sprite = *screen.getSprite();

	sprite.clear(TFT_TRANSPARENT);
	sprite.fillRoundRect(0, 0, 135, 60, 10, TFT_BLACK);
	sprite.drawIcon(low, 5, 15, 30, 30, 1, TFT_TRANSPARENT);

	sprite.setTextColor(TFT_WHITE);
	sprite.setTextSize(1);
	sprite.setTextFont(2);
	sprite.setCursor(screen.getTotalX() + 55, screen.getTotalY() + 12);
	sprite.print("Warning!");
	sprite.setCursor(screen.getTotalX() + 45, screen.getTotalY() + 32);
	sprite.print("Battery low");
}

void WarningPopup::start(){
	LoopManager::addListener(this);
	draw();
	screen.commit();
}

void WarningPopup::stop(){
	LoopManager::removeListener(this);
}

void WarningPopup::loop(uint micros){
	warningTimer += micros;
	if(warningTimer >= warningTime * 1000000){
		warningTimer = 0;
		ModalTransition* transition = static_cast<ModalTransition*>((void*)pop());
		if(prevModal == nullptr) return;
		transition->setDoneCallback([](Context* currentContext, Modal*){
			instance->prevModal->push(currentContext);
		});
	}
}

void WarningPopup::returned(void *data){
	prevModal = (Modal*)data;
}
