#include "SliderElement.h"

MiniMenu::SliderElement::SliderElement(ElementContainer* parent) : CustomElement(parent, 60, 20){

}

void MiniMenu::SliderElement::moveSliderValue(int8_t value){
	sliderValue = constrain(sliderValue + 10*value, 0, 255);
}

void MiniMenu::SliderElement::setSliderValue(uint8_t sliderValue){
	SliderElement::sliderValue = sliderValue;
}

uint8_t MiniMenu::SliderElement::getSliderValue() const{
	return sliderValue;
}

void MiniMenu::SliderElement::draw(){
	float movingCursor;
	if(sliderValue == 0){
		movingCursor = 0;
	}else{
		movingCursor = ((float) sliderValue / 255) * 41.0f;
	}
	getSprite()->drawRect(getTotalX() + 10, getTotalY() + 8, 2, 5, TFT_WHITE);
	getSprite()->drawRect(getTotalX() + 53, getTotalY() + 8, 2, 5, TFT_WHITE);
	getSprite()->drawRect(getTotalX() + 10, getTotalY() + 10, 43, 1, TFT_WHITE);
	getSprite()->fillRoundRect(getTotalX() + 10 + movingCursor, getTotalY() + 7, 4, 7, 1, TFT_WHITE);

}

