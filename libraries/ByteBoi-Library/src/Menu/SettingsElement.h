#ifndef BYTEBOI_LIBRARY_SETTINGSELEMENT_H
#define BYTEBOI_LIBRARY_SETTINGSELEMENT_H

#include <UI/CustomElement.h>
#include <functional>

namespace MiniMenu {
class SettingsElement : public CustomElement {
public:

	/**
	 * @param onChange called on click or value change, with a single parameter which represents the current value of the element
	 */
	SettingsElement(ElementContainer* parent, String name, std::function<void(int)> onChange);

	void setIsSelected(bool isSelected);

	void draw();

	/**
	 *	Called on button A click.
	 */
	virtual void click() = 0;

	/**
	 * Called on button left click.
	 */
	virtual void left() = 0;

	/**
	 * Called on button right click
	 */
	virtual void right() = 0;

	bool isSelected() const;

	void loop(uint micros);

private:
	bool selected = false;
	String name;
	float selectAccum = 0;
	int selectX = 0;

protected:
	virtual void drawControl() = 0;

	const std::function<void(int)> onChange;
};
}

#endif //BYTEBOI_LIBRARY_SETTINGSELEMENT_H
