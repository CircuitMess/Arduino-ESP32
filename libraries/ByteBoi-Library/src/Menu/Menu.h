#ifndef BYTEBOI_LIBRARY_MENU_H
#define BYTEBOI_LIBRARY_MENU_H

#include <Display/Display.h>
#include <UI/Screen.h>
#include <UI/LinearLayout.h>
#include <UI/TextElement.h>
#include <Loop/LoopListener.h>
#include "Switch.h"
#include "SliderElement.h"
#include <Support/Modal.h>
class Sprite;

namespace MiniMenu {
	class Menu : public LoopListener, public Modal {
	public:
		Menu(Context* currentContext);
		virtual ~Menu();
		void loop(uint micros) override;
		void draw() override;

		void start() override;
		void stop() override;

		void returned(void* data) override;
		static void popIntoPrevious();

	private:
		static Menu* instance;

		Sprite* canvas;
		Context* currentContext = nullptr;

		LinearLayout *layout;
		TextElement *exit = nullptr;
		LinearLayout *RGBEnableLayout;
		LinearLayout *volumeLayout;
		TextElement *ledText;
		TextElement *volumeText;
		Switch *LEDSwitch;
		SliderElement* volumeSlider;

		TextElement* texts[3];

		uint32_t blinkTime = 0;

		float selectAccum = 0;
		uint8_t selectedX;
		int8_t selectedElement = 0;
		void selectElement(uint8_t index);

		void buildUI();
		void bindInput();
		void releaseInput();

		Modal* prevModal = nullptr;
	};
};

#endif //BYTEBOI_LIBRARY_MENU_H
