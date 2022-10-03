#ifndef BYTEBOI_H
#define BYTEBOI_H

#include <Arduino.h>
#include <CircuitOS.h>
#include <Display/Display.h>
#include <Display/Sprite.h>
#include <Input/I2cExpander.h>
#include <Input/InputI2C.h>
#include <vector>
#include <FS.h>
#include <Input/InputListener.h>
#include "Pins.hpp"
#include "Menu/Menu.h"
#include "Playback/PlaybackSystem.h"
#include "Playback/Sample.h"
#include "ByteBoiLED.h"
#include "Settings.h"
#include "Battery/BatteryService.h"
#include "Battery/BatteryPopupService.h"
#include "SleepService.h"
#include "ByteBoiDisplay.h"

class ByteBoiImpl : public InputListener, public LoopListener{
public:

	/**
	 * Initializes display, Playback, Battery, I2C expander, I2C input, and pre-registers all buttons.
	 */
	void begin();
	Display* getDisplay();
	I2cExpander* getExpander();
	Input* getInput();
	static bool inFirmware();
	static bool isStandalone();

	void checkSD();
	bool sdDetected();

	/**
	 * Retrieve the path of the game directory on the SD card.
	 * @return Empty string if the game is standalone.
	 */
	String getSDPath();

	/**
	 * Open resource file. If calling this function from a standalone game, will return the file on SPIFFS located at the specified path.
	 * When a game is started through the launcher, all the resources for the game will be copied to SPIFFS into the /game directory. If
	 * called in a game started through the launcher, this function will then return the file located at /game<path>. Please note that
	 * SPIFFS file path is limited to 32 characters, including directories and slashes.
	 * @param path Path of the resource file. Make sure to use a leading slash.
	 * @param mode
	 * @return
	 */
	File openResource(const String& path, const char* mode = "r");

	/**
	 * Open data file. Intended for save files, highscores, and other data that should persist reboot. Opened file will be on SPIFFS at
	 * /data/<ID><path>. Use ByteBoi.setGameID to set the ID. If the ID isn't set, this function will return an unopened file. Please
	 * note that SPIFFS file path is limited to 32 characters, including directories and slashes.
	 * @param path Path of the data file. Make sure to use a leading slash.
	 * @param mode
	 * @return
	 */
	File openData(const String& path, const char* mode = "r");

	/**
	 * Remove data file. Intended for save files, highscores, and other data that should persist reboot. Will delete file on SPIFFS at
	 * /data/<ID><path>. Use ByteBoi.setGameID to set the ID. If the ID isn't set, this function will return false. Please
	 * note that SPIFFS file path is limited to 32 characters, including directories and slashes.
	 * @param path Path of the data file. Make sure to use a leading slash.
	 * @return True if file exists, false otherwise
	 */
	bool removeData(const String& path);

	/**
	 * Set game ID. Used for ByteBoi.openData. If more than 5 characters are passed, the ID will be truncated to 5 characters. Make sure
	 * the ID is unique for the game to avoid overwriting save data for another game.
	 * @param ID Unique identifier for the game.
	 */
	void setGameID(String ID);
	void backToLauncher();
	void bindMenu();
	void unbindMenu();
	void openMenu();

	void splash(void (* callback)() = nullptr);
	void shutdown();
	void fadeout();
	void setBacklight(bool on);

	static const char* SPIFFSgameRoot;
	static const char* SPIFFSdataRoot;

	void loop(uint micros) override;

	int getPin(uint8_t index) const;

private:
	Display* display;
	I2cExpander* expander = nullptr;
	Input* input;
	String gameID = "";
	void buttonPressed(uint i) override;
	volatile bool menuBind = false;
	static MiniMenu::Menu* popupMenu;

	void (* splashCallback)() = nullptr;
	uint32_t splashTime = 0;

	ByteBoiDisplay displayConfig;

	bool sdInserted = false;

	std::array<int, 7> pinMap;
	void initPins1();
	void initPins2();

};

extern ByteBoiImpl ByteBoi;
extern BatteryService Battery;
extern BatteryPopupService BatteryPopup;

#endif //BYTEBOI_H
