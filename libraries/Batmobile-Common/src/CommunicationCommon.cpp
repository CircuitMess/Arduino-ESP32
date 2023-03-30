#include "CommunicationCommon.h"
#include <Loop/LoopManager.h>

CommunicationCommon::CommunicationCommon() : data(1024){
	WithListeners<DisconnectListener>::reserve(16);
}

void CommunicationCommon::removeDcListener(DisconnectListener* listener){
	WithListeners<DisconnectListener>::removeListener(listener);
}

bool CommunicationCommon::isConnected(){
	return isWiFiConnected() && isClientConnected();
}

void CommunicationCommon::setClient(std::unique_ptr<AsyncClient> aClient){
	// TODO: is this a good idea?
	if(client){
		handleDisconnect();
	}

	client = std::move(aClient);
	if(!client) return;

	client->onDisconnect([this](void*, AsyncClient* server){
		Serial.println("client disconnected from port");
		handleDisconnect();
	});

	client->onData([this](void* arg, AsyncClient* server, void* data, size_t len){
		this->data.write(static_cast<uint8_t*>(data), len);
	}, nullptr);

	client->onError([this](void*, AsyncClient* c, int8_t error){
		Serial.print("error occurred: ");
		Serial.println(c->errorToString(error));
		handleDisconnect();
	}, nullptr);

	client->onTimeout([this](void*, AsyncClient*, uint32_t time){
		Serial.printf("timeout error %d passed\n", time);
		handleDisconnect();
	}, nullptr);

	client->setAckTimeout(3000);

	onConnect();
	LoopManager::addListener(this);

	WithListeners<DisconnectListener>::iterateListeners([](DisconnectListener* listener){
		listener->onConnected();
	});
}

void CommunicationCommon::addDcListener(DisconnectListener* listener){
	WithListeners<DisconnectListener>::addListener(listener);
}

void CommunicationCommon::sendPacket(const ControlPacket& packet){
	if(!client) return;
	if(!client->canSend()) return;

	client->add(reinterpret_cast<const char*>(&packet), sizeof(ControlPacket));
	client->send();
}

bool CommunicationCommon::isClientConnected(){
	if(!client) return false;
	return client->connected();
}

void CommunicationCommon::loop(uint micros){
	onLoop(micros);

	if(!isConnected() && !disconnectHandled){
		Serial.printf("wifi: %d, client: %d\n", isWiFiConnected(), isClientConnected());
		if(!client) Serial.println("client nullptr");
		else Serial.printf("client->connected: %d\n", client->connected());
		handleDisconnect();
	}

	if(disconnectHandled){
		disconnectHandled = false;
		client.reset();
		LoopManager::removeListener(this);
		return;
	}

	if(data.readAvailable() < sizeof(ControlPacket)) return;

	ControlPacket packet;
	while(data.readAvailable() >= sizeof(ControlPacket)){
		data.read(reinterpret_cast<uint8_t*>(&packet), sizeof(ControlPacket));
		processPacket(packet);
	}
}

void CommunicationCommon::handleDisconnect(){
	if(disconnectHandled) return;

	if(client) client->close(true);
	WithListeners<DisconnectListener>::iterateListeners([](DisconnectListener* listener){
		listener->onDisconnected();
	});
	onDisconnect();
	disconnectHandled = true;
}

void CommunicationCommon::end(){
	if(client){
		client->close(true);
		client.reset();
	}
}

void CommunicationCommon::setMode(ComMode mode){
	if(this->mode == mode) return;

	if(isConnected()){
		end();
	}
	this->mode = mode;
	begin();
}

