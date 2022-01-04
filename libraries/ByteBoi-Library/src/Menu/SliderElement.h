#ifndef BYTEBOI_LIBRARY_SLIDERELEMENT_H
#define BYTEBOI_LIBRARY_SLIDERELEMENT_H

#include <UI/CustomElement.h>

namespace MiniMenu {
	class SliderElement : public CustomElement {
	public:
		SliderElement(ElementContainer* parent);

		void moveSliderValue(int8_t value);

		void setSliderValue(uint8_t sliderValue);

		uint8_t getSliderValue() const;

		void draw();

	private:

		bool sliderIsSelected = false;

		uint8_t sliderValue = 255;

	};

}

#endif //BYTEBOI_LIBRARY_SLIDERELEMENT_H
