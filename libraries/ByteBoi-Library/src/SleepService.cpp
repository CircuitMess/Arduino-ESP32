#include <Loop/LoopManager.h>
#include "SleepService.h"
#include "Settings.h"
#include "ByteBoi.h"

SleepService Sleep;

void SleepService::loop(uint time){
	if(Settings.get().shutdownTime != 0){
		autoShutdownMicros += time;
		if(autoShutdownMicros >= Settings.get().shutdownTime*1000000*60){
			ByteBoi.shutdown();
			return;
		}
	}
	if(lastShutdownTime == (uint32_t)-1){
		autoShutdownMicros = 0;
		lastShutdownTime = Settings.get().shutdownTime;
	}
	if(lastShutdownTime != Settings.get().shutdownTime){
		autoShutdownMicros = 0;
	}
}

void SleepService::anyKeyPressed(){
	autoShutdownMicros = 0;
}

void SleepService::begin(){
	LoopManager::addListener(this);
	Input::getInstance()->addListener(this);
}

void SleepService::stop(){
	autoShutdownMicros = 0;
	lastShutdownTime = (uint32_t)-1;
	LoopManager::removeListener(this);
	Input::getInstance()->removeListener(this);
}
