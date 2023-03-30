#include "InfoSender.h"
#include "Communication.h"
#include "../Batmobile.h"
#include <Loop/LoopManager.h>
#include <esp_wifi_types.h>
#include <esp_wifi.h>

void InfoSender::begin(){
	LoopManager::addListener(this);
}

InfoSender::~InfoSender(){
	LoopManager::removeListener(this);
}

void InfoSender::loop(uint micros){
	batteryTime += micros;
	signalTime += micros;

	if(batteryTime >= BatteryInterval){
		batteryTime = 0;
		Com.sendBattery(Battery.getPercentage(), Battery.charging());
	}

	if(signalTime >= SignalInterval){
		signalTime = 0;
		Com.sendSignalStrength(getSignalStrength());
	}
}

uint8_t InfoSender::getSignalStrength(){
	wifi_ap_record_t info;
	uint8_t percentage = 0;

	if(esp_wifi_sta_get_ap_info(&info) == ESP_OK){
		auto con = constrain(info.rssi, MinSS, MaxSS);
		percentage = map(con, MinSS, MaxSS, 0, 100);
	}

	return percentage;
}
