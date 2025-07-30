#ifndef BYTEBOI_LIBRARY_SWITCH_H
#define BYTEBOI_LIBRARY_SWITCH_H

#include "SettingsElement.h"
#include <Loop/LoopListener.h>

namespace MiniMenu {
	class Switch : public SettingsElement, public LoopListener {
	public:
		Switch(ElementContainer* parent, String name, std::function<void(int)> onChange);

		void loop(uint micros) override;
		void click() override;

		void left() override;

		void right() override;
		void toggle();


		void set(bool state, bool instant = false);
		bool getState();

	private:
		void drawControl() override;
		bool state = false;
		bool toggling = false;
		float toggleProgress = 0;
		float toggleAccum = 0;

	};
};


#endif //BYTEBOI_LIBRARY_SWITCH_H
