#ifndef BATMOBILE_COMMON_COMMUNICATIONCOMMON_H
#define BATMOBILE_COMMON_COMMUNICATIONCOMMON_H

#include <Loop/LoopListener.h>
#include <AsyncTCP.h>
#include "ComType.h"
#include "DisconnectListener.h"
#include <Buffer/RingBuffer.h>
#include <memory>
#include <unordered_set>
#include <Util/WithListeners.h>

struct ControlPacket {
	ComType type;
	uint8_t data;
};

enum class ComMode : uint8_t {
	Direct, External
};

class CommunicationCommon : private LoopListener, private WithListeners<DisconnectListener> {
public:
	CommunicationCommon();

	virtual void begin(){};
	void end();
	void setClient(std::unique_ptr<AsyncClient> client);

	bool isConnected();

	void addDcListener(DisconnectListener* listener);
	void removeDcListener(DisconnectListener* listener);

	/**
 * Sets Communication mode, defaults to Direct mode
 * @param mode Direct mode (default) - host a WiFi AP, External mode - connect to an existing WiFi network
 */
	void setMode(ComMode mode);

protected:
	void sendPacket(const ControlPacket& packet);
	virtual bool isWiFiConnected() = 0;
	virtual void processPacket(const ControlPacket& packet) = 0;

	virtual void onLoop(uint micros){};

	ComMode mode = ComMode::Direct;

private:
	void loop(uint micros) override final;
	bool isClientConnected();
	void handleDisconnect();
	std::unique_ptr<AsyncClient> client;
	RingBuffer data;

	virtual void onConnect(){};

	virtual void onDisconnect(){};

	bool disconnectHandled = false;
};

#endif //BATMOBILE_COMMON_COMMUNICATIONCOMMON_H
