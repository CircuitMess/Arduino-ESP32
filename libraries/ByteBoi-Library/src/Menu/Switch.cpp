#include <Loop/LoopManager.h>
#include "Switch.h"

MiniMenu::Switch::Switch(ElementContainer* parent, String name, std::function<void(int)> onChange) : SettingsElement(parent, name, onChange){

}

float easeOutElastic(float x, float toggleAcum){
	float c4 = (2.0F * M_PI) / 3.0F;
	return x == 0 ? 0 : x == 1 ? 1 : pow(2, -10.0f * x / (max(0.1f, toggleAcum) * 10.0f)) * sin((x * 10.0f - 0.75f) * c4) * (0.5f + toggleAcum) + 1.0f;
}

void MiniMenu::Switch::drawControl(){
	Sprite* canvas = getSprite();

	const int offsetX = 55;

	float t = state ? easeOutElastic(toggleProgress, toggleAccum) : (1.0f - easeOutElastic(1.0f - toggleProgress, toggleAccum));
	uint hWidth = getWidth() / 2;
	uint lWidth = (0.5f - min(0.5f, t)) * 2.0f * hWidth;
	uint rWidth = (max(0.5f, t) - 0.5f) * 2 * hWidth;

	canvas->fillRect(offsetX + getTotalX(), getTotalY() + 2, hWidth - lWidth, getHeight() - 4, TFT_DARKGREY);
	canvas->fillRect(offsetX + getTotalX() + hWidth - lWidth, getTotalY() + 2, lWidth, getHeight() - 4, TFT_RED);

	canvas->fillRect(offsetX + getTotalX() + hWidth + rWidth, getTotalY() + 2, hWidth - rWidth, getHeight() - 4, TFT_DARKGREY);
	canvas->fillRect(offsetX + getTotalX() + hWidth, getTotalY() + 2, rWidth, getHeight() - 4, TFT_GREEN);

	canvas->fillRect(offsetX + getTotalX() + 2, getTotalY(), getWidth() - 4, 2, TFT_WHITE);
	canvas->fillRect(offsetX + getTotalX() + 2, getTotalY() + getHeight() - 2, getWidth() - 4, 2, TFT_WHITE);
	canvas->fillRect(offsetX + getTotalX(), getTotalY() + 2, 2, getHeight() - 4, TFT_WHITE);
	canvas->fillRect(offsetX + getTotalX() + getWidth() - 2, getTotalY() + 2, 2, getHeight() - 4, TFT_WHITE);
	canvas->fillRect(offsetX + getTotalX() + getWidth() / 2 - 1, getTotalY(), 2, getHeight(), TFT_WHITE);

	canvas->setTextSize(1);
	canvas->setTextFont(0);
	canvas->setTextColor(TFT_WHITE);

	canvas->setCursor(offsetX + getTotalX() + 7, getTotalY() + 7);
	canvas->print("OFF");
	canvas->setCursor(offsetX + getTotalX() + hWidth + 9, getTotalY() + 7);
	canvas->print("ON");
}

void MiniMenu::Switch::loop(uint micros){
	if(state){
		toggleProgress = min(1.0f, toggleProgress + (float) micros / 1000000.0f);
	}else{
		toggleProgress = max(0.0f, toggleProgress - (float) micros / 1000000.0f);
	}

	toggleAccum = max(0.0f, toggleAccum - (float) micros * 0.1f / 1000000.0f);

	if((state && toggleProgress >= .55f) || (!state && toggleProgress <= .45f)){
		toggleProgress = lround(toggleProgress);
		LoopManager::removeListener(this);
		toggling = false;
		toggleAccum = 0;
	}
}

void MiniMenu::Switch::toggle(){
	//if(toggling) return;

	state = !state;
	if(onChange){
		onChange(state);
	}
	toggleAccum = min(0.8f, toggleAccum + 0.05f);

	if(toggling){
		toggleProgress = state ? 0.05f : 1.0f - 0.05f;
	}else{
		toggling = true;
		LoopManager::addListener(this);
	}
}

void MiniMenu::Switch::set(bool state, bool instant){
	if(this->state == state) return;

	this->state = state;

	if(instant){
		if(toggling){
			LoopManager::removeListener(this);
			toggling = false;
		}

		toggleProgress = state;
	}else if(toggling){
		toggleProgress = state ? 0.05f : 1.0f - 0.05f;
	}else if(!toggling){
		toggling = true;
		LoopManager::addListener(this);
	}
}

bool MiniMenu::Switch::getState(){
	return state;
}

void MiniMenu::Switch::click(){
	toggle();
}

void MiniMenu::Switch::left(){
	toggle();
}

void MiniMenu::Switch::right(){
	toggle();
}
