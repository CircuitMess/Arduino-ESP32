#include "TextElement.h"


MiniMenu::TextElement::TextElement(ElementContainer* parent, String name, std::function<void(int)> onChange) : SettingsElement(parent, name, onChange){

}

void MiniMenu::TextElement::drawControl(){

}

void MiniMenu::TextElement::click(){
	onChange(0);
}

void MiniMenu::TextElement::left(){

}

void MiniMenu::TextElement::right(){

}
