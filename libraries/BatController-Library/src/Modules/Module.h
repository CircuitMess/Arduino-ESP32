#ifndef BATCONTROLLER_FIRMWARE_MODULE_H
#define BATCONTROLLER_FIRMWARE_MODULE_H

#include <Loop/LoopListener.h>

class Module : private LoopListener {
public:
	explicit Module(const uint8_t& addr);
	~Module() override;

	/**
	 * Attempt connection and initialize module.
	 */
	void begin();
	void end();

	[[nodiscard]] bool isConnected() const;

protected:
	/**
	 * Needs to be called in implementations when an i2c transmission error occurs.
	 * Start looking for a new connection afterwards.
	 */
	void errorOccured();

	/**
	 * Initialize module to default state, called when connected.
	 */
	virtual void init() = 0;

	/**
	 * Read/write data from module, called periodically.
	 */
	virtual void transmission() = 0;
	const uint8_t address = 0;

private:
	void loop(uint micros) override;
	void checkConnection();

	bool connected = false;
	static constexpr uint32_t CheckInterval = 100000;
	static constexpr uint32_t ReadInterval = 10000;
	uint32_t checkCounter = 0;
	uint32_t transmissionCounter = 0;

};


#endif //BATCONTROLLER_FIRMWARE_MODULE_H
