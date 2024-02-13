#ifndef BATMOBILE_LIBRARY_S3INTERFACE_H
#define BATMOBILE_LIBRARY_S3INTERFACE_H

#include <Arduino.h>
#include <SPI.h>
#include <DriveInfo.h>
#include <memory>
#include <FS.h>
#include <Buffer/RingBuffer.h>

enum class S3Error {
	None, Camera, Update
};

class S3Interface {
public:
	S3Interface();

	bool begin();
	bool end();

	void reset();

	S3Error getError();
	uint8_t getVersion();

	/**
	 * Flashes a firmware image onto S3. WARNING: Flashing a corrupted or incompatible image
	 * will brick the device. This function is disabled (returns early) for the time being.
	 * @param file Firmware image file.
	 */
	void update(File file);

	void setMode(DriveMode mode);
	std::unique_ptr<DriveInfo> getFrame();
	ProximityData getProximity();
	void setHue(uint8_t hue);
	void setShake(bool shake);

	void getFrameGray(uint8_t* buffer);

private:
	void initPins();

	DriveMode state = DriveMode::Idle;

	SPIClass SPI;
	const SPISettings SS;

	std::unique_ptr<uint8_t> recvBuf;
	RingBuffer recvRing;

	void send(uint8_t byte1, uint8_t byte2 = 0, uint8_t byte3 = 0, uint8_t byte4 = 0);
	void sendData(const void* data, size_t size);
	uint8_t recv();
	void recvData(void* data, size_t count);
	void clearRecv(size_t size);
	void waitReady();

	static constexpr size_t MaxFrameSize = 10000;

	class SPITransaction {
	public:
		SPITransaction(SPIClass& spi, const SPISettings SS);
		virtual ~SPITransaction();

		void send(const void* data, size_t size);
		void recv(void* data, size_t size);
		void clear(size_t size);

		void end();

	private:
		SPIClass& SPI;
		bool active = true;

	};

};


#endif //BATMOBILE_LIBRARY_S3INTERFACE_H
