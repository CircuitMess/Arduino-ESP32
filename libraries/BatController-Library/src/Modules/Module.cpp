#include "Module.h"
#include <Wire.h>
#include <Loop/LoopManager.h>

Module::Module(const uint8_t& addr) : address(addr){
}

Module::~Module(){
	end();
}

void Module::begin(){
	checkConnection();

	if(isConnected()){
		init();
		transmissionCounter = 0;
	}

	LoopManager::addListener(this);
}

void Module::end(){
	LoopManager::removeListener(this);
}

bool Module::isConnected() const{
	return connected;
}

void Module::checkConnection(){
	Wire.beginTransmission(address);
	connected = Wire.endTransmission() == 0;
}

void Module::loop(uint micros){
	checkCounter += micros;
	if(checkCounter >= CheckInterval){
		checkCounter = 0;
		bool prevConnected = connected;
		checkConnection();
		if(isConnected() && !prevConnected){
			init();
			transmissionCounter = 0;
			return;
		}
	}

	if(!connected) return;
	transmissionCounter += micros;
	if(transmissionCounter >= ReadInterval){
		transmissionCounter = 0;
		transmission();
	}
}


void Module::errorOccured(){
	connected = false;
	checkCounter = 0;
	transmissionCounter = 0;
}




