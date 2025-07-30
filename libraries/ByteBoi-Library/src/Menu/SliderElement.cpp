#include "SliderElement.h"

MiniMenu::SliderElement::SliderElement(ElementContainer* parent, String name, std::function<void(int)> onChange) : SettingsElement(parent, name, onChange){

}

void MiniMenu::SliderElement::click(){
	if(sliderValue == 0){
		setSliderValue(180);
	}else{
		setSliderValue(0);
	}
	onChange(sliderValue);
}

void MiniMenu::SliderElement::left(){
	moveSliderValue(-1);
}

void MiniMenu::SliderElement::right(){
	moveSliderValue(1);
}

void MiniMenu::SliderElement::moveSliderValue(int8_t value){
	sliderValue = constrain(sliderValue + 10 * value, 0, 255);
	onChange(sliderValue);
}

void MiniMenu::SliderElement::setSliderValue(uint8_t sliderValue){
	SliderElement::sliderValue = sliderValue;
}

uint8_t MiniMenu::SliderElement::getSliderValue() const{
	return sliderValue;
}

void MiniMenu::SliderElement::drawControl(){
	float movingCursor;
	if(sliderValue == 0){
		movingCursor = 0;
	}else{
		movingCursor = ((float) sliderValue / 255) * 41.0f;
	}

	static const int offsetX = 55;
	getSprite()->drawRect(getTotalX() + 10 + offsetX, getTotalY() + 8, 2, 5, TFT_WHITE);
	getSprite()->drawRect(getTotalX() + 53 + offsetX, getTotalY() + 8, 2, 5, TFT_WHITE);
	getSprite()->drawRect(getTotalX() + 10 + offsetX, getTotalY() + 10, 43, 1, TFT_WHITE);
	getSprite()->fillRoundRect(getTotalX() + 10 + offsetX + movingCursor, getTotalY() + 7, 4, 7, 1, TFT_WHITE);

}
