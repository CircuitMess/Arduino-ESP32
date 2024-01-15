#include "Settings.h"
#include <Util/Settings.h>

SettingsImpl Settings;

const uint32_t SleepSeconds[SLEEP_STEPS] = { 0, 30, 60, 2 * 60, 5 * 60 };
const char* SleepText[SLEEP_STEPS] = { "OFF", "30 sec", "1 min", "2 min", "5 min" };

const uint32_t ShutdownSeconds[SHUTDOWN_STEPS] = { 0, 60*60, 6 * 60*60, 24 * 60*60, 2 * 24 * 60*60 };
const char* ShutdownText[SHUTDOWN_STEPS] = { "OFF", "1 hour", "6 hours", "1 day", "2 days" };

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