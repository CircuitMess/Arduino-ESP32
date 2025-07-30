#ifndef BYTEBOI_LIBRARY_TEXTELEMENT_H
#define BYTEBOI_LIBRARY_TEXTELEMENT_H

#include "SettingsElement.h"

namespace MiniMenu {
class TextElement : public SettingsElement {
public:
	TextElement(ElementContainer* parent, String name, std::function<void(int)> onChange);

	void click() override;

private:
	void left() override final;
	void right() override final;

protected:
	void drawControl() override;
};
}

#endif //BYTEBOI_LIBRARY_TEXTELEMENT_H
