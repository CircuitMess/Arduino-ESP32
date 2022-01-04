#include "SettingsElement.h"

MiniMenu::SettingsElement::SettingsElement(ElementContainer* parent, String name) : CustomElement(parent, 160, 20), name(name){

}

void MiniMenu::SettingsElement::setIsSelected(bool isSelected){
	SettingsElement::selected = isSelected;

}

void MiniMenu::SettingsElement::draw(){
	getSprite()->setTextColor(TFT_WHITE);
	getSprite()->setTextSize(1);
	getSprite()->setTextFont(1);
	getSprite()->setCursor(getTotalX() + 5, getTotalY() + 10);
	getSprite()->println(name);
	if(selected){
		getSprite()->drawRect(getTotalX() + 3, getTotalY() + 6, 86, 15, TFT_WHITE);
	}
	drawControl();
}

bool MiniMenu::SettingsElement::isSelected() const{
	return selected;
}
