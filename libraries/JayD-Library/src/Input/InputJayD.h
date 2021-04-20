#ifndef JAYD_INPUTJAYD_H
#define JAYD_INPUTJAYD_H


#include <Loop/LoopListener.h>
#include <Util/Vector.h>
#include <sys/param.h>
#include <vector>
#include <Util/Vector.h>

#define BYTE_IDENTIFY 0x0
#define JDNV_ADDR 0x43
#define BYTE_NUMEVENTS 0x12
#define BYTE_GETEVENTS 0x13
#define BYTE_GETPOTVALUE 0x14
#define NUM_BTN 9
#define NUM_ENC 7
#define NUM_POT 3
#define JDNV_PIN_RESET 13


enum DeviceType {
	BTN, ENC, POT
};

struct InputEvent {
	DeviceType deviceType;
	uint8_t deviceID;
	int8_t value;
};

class InputJayD;

class JayDInputListener {
	friend InputJayD;

private:
	virtual void buttonPress(uint8_t id);
	virtual void buttonRelease(uint8_t id);
	virtual void buttonHold(uint8_t id);

	virtual void encoderMove(uint8_t id, int8_t value);
	virtual void potMove(uint8_t id, uint8_t value);

};

class InputJayD : public LoopListener {

public:

	InputJayD();

	virtual void setBtnPressCallback(uint8_t id, void(*callback)());
	virtual void setBtnReleaseCallback(uint8_t id, void (*callback)());
	virtual void removeBtnPressCallback(uint8_t id);
	virtual void removeBtnReleaseCallback(uint8_t id);
	virtual void setBtnHeldCallback(uint8_t id, uint32_t holdTime, void (*callback)());
	virtual void removeBtnHeldCallback(uint8_t id);

	virtual void setEncoderMovedCallback(uint8_t id, void (*callback)(int8_t value));
	virtual void removeEncoderMovedCallback(uint8_t id);

	virtual void setPotMovedCallback(uint8_t id, void (*callback)(uint8_t value));
	virtual void removePotMovedCallback(uint8_t id);

	uint8_t getPotValue(uint8_t potID);

	static InputJayD *getInstance();

	void reset();
	bool begin();
	bool identify();

	void addListener(JayDInputListener* listener);
	void removeListener(JayDInputListener* listener);

	void loop(uint _time) override;

	void setHoldTime(uint32_t holdTime);

private:

	std::vector<void (*)()> btnPressCallbacks;
	std::vector<void (*)()> btnReleaseCallbacks;
	std::vector<void (*)()> btnHoldCallbacks;
	std::vector<void (*)(int8_t)> encMovedCallbacks;
	std::vector<void (*)(uint8_t)> potMovedCallbacks;

	std::vector <uint32_t> btnHoldValue;
	std::vector <uint32_t> btnHoldStart;
	std::vector<bool> wasPressed;

	Vector<JayDInputListener*> listeners;

	static InputJayD *instance;

	uint8_t getNumEvents();

	void fetchEvents(int numEvents);

	void handleButtonEvent(uint8_t id, uint8_t value);
	void handleEncoderEvent(uint8_t id, int8_t value);
	void handlePotentiometerEvent(uint8_t id, uint8_t value);

	void buttonHoldCheck();

	uint32_t currentTime = 0;
	uint32_t encoderTime = 0;
	int8_t tempEncValue[7] = {0};

	uint32_t holdTime = 0;

};


#endif //JAYD_LIBRARY_INPUTJAYD_H
