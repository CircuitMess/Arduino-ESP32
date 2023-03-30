#include "Batmobile.h"
#include <Loop/LoopManager.h>
#include <esp_wifi.h>

BatmobileImpl Batmobile;
MotorControl Motors;
BatteryService Battery;
S3Interface S3;
AudioSystem Audio;
SingleLEDController Headlights(LED_FRONT, 0);
SingleLEDController Taillights(LED_BACK, 1);
RGBLEDController Underlights({ LED_R, LED_G, LED_B }, { 2, 3, 4 });

BatmobileImpl::BatmobileImpl(){

}

void BatmobileImpl::begin(){
	Motors.begin();

	disableCore0WDT();
	disableCore1WDT();

	WiFi.mode(WIFI_OFF);

	if(!SPIFFS.begin()){
		Serial.println("SPIFFS error");
	}

	Headlights.begin();
	Taillights.begin();
	Underlights.begin();

	Audio.begin();
	Battery.begin();

	if(!S3.begin()){
		printf("S3 begin failed\n");
	}
}

void BatmobileImpl::shutdown() {
    Com.end();
    WiFi.disconnect(true);
	WiFi.mode(WIFI_OFF);
	esp_wifi_stop();
	adc_power_off();

    Underlights.end();
    Taillights.end();
    Headlights.end();

    Motors.end();
    Audio.stop();
    S3.end();

    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
    esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    esp_deep_sleep_start();
}

void BatmobileImpl::shutdownNotify(){
	Motors.end();
	Audio.play(SPIFFS.open("/SFX/off.aac"));

	Underlights.breathe({ 255, 0, 0 }, { 0, 0, 0 }, 6000);
	uint32_t t = millis();
	while(millis() - t <= 3000){
		Underlights.loop(0);
	}

	delay(1000);

	Batmobile.shutdown();
}

void BatmobileImpl::shutdownError(){
	Motors.end();
	Audio.play(SPIFFS.open("/SFX/error.aac"));

	uint32_t t = millis();
	while(millis() - t < 4000){
		Underlights.setSolid({ 255, 0, 0 });
		delay(500);
		Underlights.setSolid({ 0, 0, 255 });
		delay(500);
	}

	Audio.stop();
	shutdownNotify();
}
