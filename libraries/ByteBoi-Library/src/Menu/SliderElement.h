#ifndef BYTEBOI_LIBRARY_SLIDERELEMENT_H
#define BYTEBOI_LIBRARY_SLIDERELEMENT_H

#include "SettingsElement.h"


namespace MiniMenu {
class SliderElement : public SettingsElement {
public:
	SliderElement(ElementContainer* parent, String name, std::function<void(int)> onChange);

	void click() override;

	void left() override;
	void right() override;

	void setSliderValue(uint8_t sliderValue);

	uint8_t getSliderValue() const;

private:
	void moveSliderValue(int8_t value);

	bool sliderIsSelected = false;

	uint8_t sliderValue = 255;


protected:
	void drawControl() override;
};
}


#endif //BYTEBOI_LIBRARY_SLIDERELEMENT_H
