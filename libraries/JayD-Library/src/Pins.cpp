#include "Pins.h"
#include <Util/HWRevision.h>

static const char* TAG = "Pins";

Pins* Pins::instance = nullptr;

Pins::Pins(){
	initPinMaps();
}

int Pins::get(Pin pin){
	if(instance == nullptr){
		instance = new Pins();
		uint8_t revision = HWRevision::get();

		if(revision == 0 || revision == 1 || revision == 2){
			instance->currentMap = &instance->Revision1;
		} else if(revision == 3){
			instance->currentMap = &instance->Revision2;
		} else{
			while(true){
				printf("Pins: Current revision: %d\n", revision);
				while(true);
			}
		}
	}

	assert(instance != nullptr);

	PinMap* pinMap = instance->currentMap;

	if(pinMap == nullptr){
		printf("Pins: Pin map is invalid.!\n");
		return -1;
	}

	if(pinMap->find(pin) == pinMap->end()){
		printf("Pins: Pin %d not mapped!\n", (int)pin);
		return -1;
	}

	return pinMap->at(pin);
}

void Pins::setLatest(){
	if(instance == nullptr){
		instance = new Pins();
	}

	instance->currentMap = instance->pinMaps.back();
}

void Pins::setRev(const uint8_t revision){
	if(instance == nullptr){
		instance = new Pins();
	}

	if(revision == 0 || revision == 1 || revision == 2){
		instance->currentMap = &instance->Revision1;
	} else if(revision == 3){
		instance->currentMap = &instance->Revision2;
	} else{
		while(true){
			printf("Pins setRev: non-existent revision: %d\n", revision);
			while(true);
		}
	}
}

void Pins::initPinMaps(){
	Revision1 = {
		{ Pin::PIN_BL, 25 },
		{ Pin::SD_CS, 22 },
		{ Pin::I2S_WS, 4 },
		{ Pin::I2S_DO, 14 },
		{ Pin::I2S_BCK, 21 },
		{ Pin::I2S_DI, -1 },
		{ Pin::I2C_SDA, 26 },
		{ Pin::I2C_SCL, 27 },
		{ Pin::SPI_SCK, 18 },
		{ Pin::SPI_MISO, 19 },
		{ Pin::SPI_MOSI, 23 },
		{ Pin::SPI_SS, -1 },
		{ Pin::PIN_DC, 33 },
		{ Pin::PIN_CS, 32 },
		{ Pin::PIN_RST, 2 },
	};

	Revision2 = {
		{ Pin::PIN_BL, 25 },
		{ Pin::I2S_WS, 4 },
		{ Pin::I2S_DO, 19 },
		{ Pin::I2S_BCK, 21 },
		{ Pin::I2S_DI, -1 },
		{ Pin::I2C_SDA, 26 },
		{ Pin::I2C_SCL, 27 },
		{ Pin::SPI_SCK, 18 },
		{ Pin::SPI_MISO, -1 },
		{ Pin::SPI_MOSI, 23 },
		{ Pin::SPI_SS, -1 },
		{ Pin::PIN_DC, 33 },
		{ Pin::PIN_CS, 32 },
		{ Pin::PIN_RST, 12 },
	};
}
