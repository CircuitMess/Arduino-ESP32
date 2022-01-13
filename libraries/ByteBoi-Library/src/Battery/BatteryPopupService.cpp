#include "BatteryPopupService.h"
#include <Support/ContextTransition.h>
#include <Support/ModalTransition.h>
#include "ShutdownPopup.h"
#include "WarningPopup.h"
#include "BatteryService.h"
#include "../ByteBoi.h"
#include <Loop/LoopManager.h>

const uint16_t BatteryPopupService::checkInterval = 5; //in seconds
ShutdownPopup *BatteryPopupService::shutdownPopup = nullptr;
WarningPopup *BatteryPopupService::warningPopup = nullptr;

void BatteryPopupService::loop(uint time){
	if(!enabled){
		LoopManager::removeListener(this);
		return;
	}

	checkMicros += time;
	//voltage not yet read or charging
	if(Battery.getVoltage() == 0 || Battery.chargePinDetected()){
		warningShown = false;
		return;
	}
	uint8_t percentage = Battery.getPercentage();

	if(checkMicros >= checkInterval * 1000000){

		if(percentage <= 1){
			if(ContextTransition::isRunning() ||
			   (Modal::getCurrentModal() != nullptr && (Modal::getCurrentModal() == shutdownPopup || Modal::getCurrentModal() == warningPopup))
			   || ModalTransition::isRunning()) return;

			if(Modal::getCurrentModal() != nullptr){
				ModalTransition::setDeleteOnPop(false);
				ModalTransition* transition = static_cast<ModalTransition *>((void *)Modal::getCurrentModal()->pop());
				transition->setDoneCallback([](Context *currentContext, Modal *prevModal){
					shutdownPopup = new ShutdownPopup(*currentContext);
					shutdownPopup->push(currentContext);
					ModalTransition::setDeleteOnPop(true);
				});
			}else{
				shutdownPopup = new ShutdownPopup(*Context::getCurrentContext());
				shutdownPopup->push(Context::getCurrentContext());
			}
		}else if(percentage <= 15 && !warningShown){
			if(ContextTransition::isRunning() ||
			   (Modal::getCurrentModal() != nullptr && Modal::getCurrentModal() == shutdownPopup) ||
			   ModalTransition::isRunning()) return;

			warningShown = true;
			ModalTransition *transition;

			if(Modal::getCurrentModal() != nullptr){
				ModalTransition::setDeleteOnPop(false);
				transition = static_cast<ModalTransition *>((void *)Modal::getCurrentModal()->pop());
				transition->setDoneCallback([](Context *currentContext, Modal *prevModal){
					warningPopup = new WarningPopup(*currentContext);
					warningPopup->push(currentContext);
					warningPopup->returned(prevModal);
					ModalTransition::setDeleteOnPop(true);
				});
			}else{
				warningPopup = new WarningPopup(*Context::getCurrentContext());
				warningPopup->push(Context::getCurrentContext());
			}
		}
		checkMicros = 0;
	}
}

void BatteryPopupService::enablePopups(bool enable){
	if(enable){
		Battery.setAutoShutdown(false);
	}

	if(enable && !enabled){
		LoopManager::addListener(this);
	}else if(!enable && enabled){
		LoopManager::removeListener(this);
	}

	enabled = enable;
}
