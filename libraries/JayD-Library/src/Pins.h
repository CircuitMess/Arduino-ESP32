#ifndef JAYD_LIBRARY_PINS_H
#define JAYD_LIBRARY_PINS_H

#include <cstdint>
#include <unordered_map>
#include <vector>

#define PIN(x) Pins::get(Pin::x)

enum class Pin : uint8_t {
	PIN_BL,
	SD_CS,
	I2S_WS,
	I2S_DO,
	I2S_BCK,
	I2S_DI,
	I2C_SDA,
	I2C_SCL,
	SPI_SCK,
	SPI_MISO,
	SPI_MOSI,
	SPI_SS,
	PIN_DC,
	PIN_CS,
	PIN_RST
};

class Pins {
	struct PinHash {
		std::size_t operator()(Pin pin) const{
			return static_cast<std::size_t>(pin);
		}
	};

	typedef std::unordered_map<Pin, int, PinHash> PinMap;

public:
	static int get(Pin pin);

	static void setLatest();
	static void setRev(uint8_t revision);

private:
	Pins();

	PinMap* currentMap = nullptr;

	static Pins* instance;

	void initPinMaps();

	//For original Jay-D, Jay-D v1.7
	PinMap Revision1;

	//For Jay-D v1.9 (sd fix)
	PinMap Revision2;

	std::vector<PinMap*> pinMaps = { &Revision1, &Revision2 };
};

#endif //JAYD_LIBRARY_PINS_H
