#include "Spencer.h"
SpencerImpl Spencer;
LEDmatrixImpl LEDmatrix;
SpencerImpl::SpencerImpl()
{
}

SpencerImpl::~SpencerImpl()
{
}


void SpencerImpl::begin()
{
	disableCore0WDT();
	disableCore1WDT();

	SPIClass spi(3);
	spi.begin(18, 19, 23, FLASH_CS_PIN);
	if(!SerialFlash.begin(spi, FLASH_CS_PIN)){
		Serial.println("Flash fail");
		return;
	}

	pinMode(LED_PIN, OUTPUT);

	if(!LEDmatrix.begin()){
		Serial.println("couldn't start matrix");
		for(;;);
	}

	I2S* i2s = new I2S();
	i2s_driver_uninstall(I2S_NUM_0); //revert wrong i2s config from esp8266audio
	i2s->begin();

	Playback.begin(i2s);
	Recording.begin(i2s);

	LoopManager::addListener(&Playback);
	LoopManager::addListener(&LEDmatrix);
	LoopManager::addListener(new InputGPIO());

	Net.set(Settings.get().SSID, Settings.get().pass);

	LoopManager::setStackSize(10240);
}

bool SpencerImpl::loadSettings(){
	bool firstTime = !Settings.begin() || Settings.get().SSID[0] == 0;

	if(firstTime){
		Settings.reset();
		Settings.get().brightnessLevel = Settings.get().volumeLevel = 1;
		Settings.store();
	}

	uint8_t brightnessLevelValues[3] = {5, 20, 100};
	float audioLevelValues[3] = {0.1, 0.4, 1.0};
	LEDmatrix.setBrightness(brightnessLevelValues[Settings.get().brightnessLevel]);
	Playback.setVolume(audioLevelValues[Settings.get().volumeLevel]);

	return firstTime;
}
