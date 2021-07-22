#ifndef WHEELSON_LIBRARY_I2CQUEUE_H
#define WHEELSON_LIBRARY_I2CQUEUE_H


#include <Wire.h>
#include <Loop/LoopListener.h>
#include <queue>
#include <Sync/Mutex.h>

/**
 * Used for queueing i2c transfers to the main task. For some reason, i2c gets locked up if used on a core or task other than the main Arduino
 * loop, even when ensuring mutual exclusivity on i2c transfers.
 */
class I2CQueue : public LoopListener {
public:

	/**
	 * @param addr device address
	 * @param wire TwoWire interface used for transfers
	 */
	I2CQueue(uint8_t addr, TwoWire& wire = ::Wire);

	/**
	 * Set the task used for transfers.
	 */
	void setHome();

	void loop(uint micros) override;

	/**
	 * This is a non-blocking function if used outside home task. When called in home task, sends the data immediately.
	 * @param data data buffer
	 * @param n size of buffer
	 */
	void send(const void* data, size_t n);

	/**
	 * This is a blocking function. When called in home task, requests the data immediately.
	 * @param msg request message
	 * @param msgSize size of request message
	 * @param dst destination buffer
	 * @param dstSize size of destination buffer
	 */
	void request(const void* msg, size_t msgSize, void* dst, size_t dstSize);

private:
	TwoWire& Wire;
	uint8_t addr;
	TaskHandle_t homeTask = nullptr;
	Mutex transactionMutex;

	struct SendData {
		SendData(void* data, size_t n);
		void* data;
		size_t n;
	};

	struct RequestData {
		RequestData(const void* msg, size_t msgSize, void* dst, size_t n);
		const void* msg;
		size_t msgSize;
		void* dst;
		size_t dstSize;
		bool* done = nullptr;
	};

	std::vector<SendData> sendData;
	std::vector<RequestData> requestData;

	void handleSend(SendData& data);
	void handleRequest(const RequestData& data);

	bool isHome();
};


#endif //WHEELSON_LIBRARY_I2CQUEUE_H
