#include "I2CQueue.h"
#include <freertos/task.h>

I2CQueue::SendData::SendData(void* data, size_t n) : data(data), n(n){ }

I2CQueue::RequestData::RequestData(const void* msg, size_t msgSize, void* dst, size_t n) : msg(msg), msgSize(msgSize), dst(dst), dstSize(n){ }

I2CQueue::I2CQueue(uint8_t addr, TwoWire& wire) : Wire(wire), addr(addr){}

void I2CQueue::loop(uint micros){
	transactionMutex.lock();

	for(auto& data : sendData){
		handleSend(data);
	}

	for(const auto& data : requestData){
		handleRequest(data);
	}

	sendData.clear();
	requestData.clear();
	transactionMutex.unlock();
}

void I2CQueue::handleSend(I2CQueue::SendData& data){
	Wire.beginTransmission(addr);
	Wire.write((const uint8_t*) data.data, data.n);
	Wire.endTransmission();

	free(data.data);
}

void I2CQueue::handleRequest(const I2CQueue::RequestData& data){
	Wire.beginTransmission(addr);
	Wire.write((const uint8_t*) data.msg, data.msgSize);
	Wire.endTransmission();

	Wire.requestFrom(addr, data.dstSize);

	while(!Wire.available()){
		delayMicroseconds(1);
	}

	Wire.readBytes((uint8_t*) data.dst, data.dstSize);

	if(data.done){
		*data.done = true;
	}
}

void I2CQueue::send(const void* data, size_t n){
	SendData send(malloc(n), n);
	memcpy(send.data, data, n);

	if(isHome()){
		handleSend(send);
		return;
	}

	transactionMutex.lock();
	sendData.push_back(send);
	transactionMutex.unlock();
}

void I2CQueue::request(const void* msg, size_t msgSize, void* dst, size_t n){
	RequestData request(msg, msgSize, dst, n);

	if(isHome()){
		handleRequest(request);
		return;
	}

	request.done = new bool(false);

	transactionMutex.lock();
	requestData.push_back(request);
	transactionMutex.unlock();

	while(!*request.done){
		delay(1);
	}

	delete request.done;
}

bool I2CQueue::isHome(){
	return xTaskGetCurrentTaskHandle() == homeTask;
}

void I2CQueue::setHome(){
	homeTask = xTaskGetCurrentTaskHandle();
}
