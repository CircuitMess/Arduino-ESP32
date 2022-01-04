#include "ShutdownPopup.h"
#include <Loop/LoopManager.h>
#include "../ByteBoi.h"
#include "../Bitmaps/off.hpp"
#include <SPIFFS.h>

const uint8_t ShutdownPopup::shutdownTime = 5;

ShutdownPopup::ShutdownPopup(Context &context) : Modal(context, 135, 60){
	screen.getSprite()->setChroma(TFT_TRANSPARENT);
	buffer= static_cast<Color*>(malloc(sizeof(off)));
	memcpy(buffer,off,sizeof(off));

}
ShutdownPopup::~ShutdownPopup(){
	free(buffer);
}

void ShutdownPopup::draw(){
	Sprite& sprite = *screen.getSprite();

	sprite.clear(TFT_TRANSPARENT);
	sprite.fillRoundRect(0, 0, 135, 60, 10, TFT_BLACK);
	sprite.drawIcon(buffer, 5, 15, 30, 30, 1, TFT_TRANSPARENT);

	sprite.setTextColor(TFT_WHITE);
	sprite.setTextSize(1);
	sprite.setTextFont(2);
	sprite.setCursor(screen.getTotalX() + 40, screen.getTotalY() + 12);
	sprite.print("Battery empty!");
	sprite.setCursor(screen.getTotalX() + 40, screen.getTotalY() + 32);
	sprite.print("Shutting down");
}

void ShutdownPopup::start(){
	LoopManager::addListener(this);
	draw();
	screen.commit();
}

void ShutdownPopup::stop(){
	LoopManager::removeListener(this);
}

void ShutdownPopup::loop(uint micros){
	shutdownTimer += micros;
	if(shutdownTimer >= shutdownTime * 1000000){
		ByteBoi.shutdown();
	}
}

