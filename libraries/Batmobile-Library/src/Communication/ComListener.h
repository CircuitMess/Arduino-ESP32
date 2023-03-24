#ifndef BATMOBILE_LIBRARY_COMLISTENER_H
#define BATMOBILE_LIBRARY_COMLISTENER_H

#include <Arduino.h>
#include <DriveMode.h>
#include <DanceType.h>

class ComListener {
public:
	/**
	 * @param hue HSV format hue divided by 2 [0-180]
	 */
	virtual void onBallHue(uint8_t hue){};

	virtual void onBoost(bool boost){};

	/**
	 * @param dir Bitwise OR-ed values of direction buttons,
	 * lowest to highest bit represents forward, backward, left, right, respectively
	 */
	virtual void onDriveDir(uint8_t dir, uint8_t speed){};

	virtual void onDriveMode(DriveMode mode){};

	/**
	 * @param speed [0-100]
	 */
	virtual void onDriveSpeed(uint8_t speed){};

	virtual void onHonk(){};

	/**
	 * @param volume [0-100]
	 */
	virtual void onVolume(uint8_t volume){};

	/**
	 * Called specifically in Settings to indicate a change in volume while editing
	 */
	virtual void onSettingsSound(){};

	virtual void onShutdown(){};

	/**
	 * Called when Controller requests a disconnect, typically when pairing another device or factory resetting.
	 */
	virtual void onDisconnectRequest(){};

	virtual void onHeadlights(uint8_t val){};

	virtual void onTaillights(uint8_t val){};

	virtual void onUnderlights(uint8_t color){};

	virtual void onSoundEffect(uint8_t sample){};

	virtual void onDance(DanceType dance){};

	virtual void onOverrideSound(bool manual){};

	virtual void onMotorsTimeout(uint8_t duration){};

	virtual void onMotorsTimeoutClear(){};

	virtual void onIdleSounds(bool toggle){};
};


#endif //BATMOBILE_LIBRARY_COMLISTENER_H
