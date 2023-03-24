#include "Settings.h"
#include <Util/Settings.h>

SettingsImpl Settings;

const uint32_t ShutdownSeconds[SHUTDOWN_STEPS] = { 0, 60, 5 * 60, 10 * 60, 15 * 60 };
const char* ShutdownText[SHUTDOWN_STEPS] = { "OFF", "1 min", "5 min", "10 min", "15 min" };

bool SettingsImpl::begin(){
	return Settings::init(&data, sizeof(SettingsData));
}

SettingsData& SettingsImpl::get(){
	return data;
}

void SettingsImpl::store(){
	Settings::store();
}

void SettingsImpl::reset(){
	data = SettingsData{};
	SettingsImpl::store();
}