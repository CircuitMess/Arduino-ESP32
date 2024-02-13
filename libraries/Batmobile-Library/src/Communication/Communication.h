#ifndef BATMOBILE_LIBRARY_COMMUNICATION_H
#define BATMOBILE_LIBRARY_COMMUNICATION_H

#include <BatCommon.h>
#include <AsyncTCP.h>
#include <ComType.h>
#include "ComListener.h"
#include "InfoSender.h"
#include <map>
#include <set>
#include <Error.h>

class Communication : public CommunicationCommon {
public:
	Communication();
	~Communication() override;

	void begin() override;

	void sendBattery(uint8_t percent, bool charging);
	void sendSignalStrength(uint8_t percent);
	void sendShutdownAck();
	void sendError(BatError errorCode);

	void addListener(ComType type, ComListener* listener);
	void addListener(std::initializer_list<ComType> type, ComListener* listener);
	void removeListener(ComType type, ComListener* listener);
	void removeListener(ComListener* listener);

protected:
	void onLoop(uint micros) override;

private:
	bool isWiFiConnected() override;
	void processPacket(const ControlPacket& packet) override;

	void onConnect() override;
	void onDisconnect() override;

	std::map<ComType, std::unordered_set<ComListener*>> listeners;
	InfoSender infoSender;

	uint32_t heartbeatReceived = 0;
	uint32_t heartbeatCounter = 0;
	static constexpr uint32_t HeartbeatTimeout = 6000000;
};

extern Communication Com;

#endif //BATMOBILE_LIBRARY_COMMUNICATION_H
