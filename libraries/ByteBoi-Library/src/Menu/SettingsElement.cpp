#include "SettingsElement.h"

MiniMenu::SettingsElement::SettingsElement(ElementContainer* parent, String name, std::function<void(int)> onChange) : CustomElement(parent, 60, 20), name(name), onChange(onChange){

}

void MiniMenu::SettingsElement::setIsSelected(bool isSelected){
	SettingsElement::selected = isSelected;
}

void MiniMenu::SettingsElement::draw(){
	getSprite()->setTextSize(1);
	getSprite()->setTextFont(1);
	if(selected){
		int8_t newX = getTotalX() + 5 + sin(selectAccum * 5.0f) * 3.0f;
		getSprite()->setTextColor(TFT_YELLOW);
		getSprite()->setCursor(newX, getTotalY() + 7);
	}else{
		getSprite()->setTextColor(TFT_WHITE);
		getSprite()->setCursor(getTotalX() + 5, getTotalY() + 7);
	}
	getSprite()->println(name);


	drawControl();
}

bool MiniMenu::SettingsElement::isSelected() const{
	return selected;
}

void MiniMenu::SettingsElement::loop(uint micros){
	selectAccum += (float) micros / 1000000.0f;
}