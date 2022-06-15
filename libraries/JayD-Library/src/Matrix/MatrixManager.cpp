#include <Loop/LoopManager.h>
#include <SPIFFS.h>
#include "MatrixManager.h"
#include <Devices/Matrix/MatrixAnimGIF.h>

const char* MatrixManager::PartitionNames[] = { "left", "right", "mid", "big" };

MatrixManager::MatrixManager(MatrixOutputBuffer* buf) : matrixBigOut(buf), matrixLOut(buf), matrixROut(buf), matrixMidOut(buf), matrixBig(matrixBigOut),
														matrixL(matrixLOut), matrixR(matrixROut), matrixMid(matrixMidOut){

	partitions[0] = &matrixL;
	partitions[1] = &matrixR;
	partitions[2] = &matrixMid;
	partitions[3] = &matrixBig;
}

void MatrixManager::loop(uint time){
	if(!playingRandom) return;
	if(matrixL.getAnimations().empty()) return;

	if(anims[0]->getGIF().getLoopCount() >= 1){
		playRandom();
	}
}

void MatrixManager::push(){
	matrixR.push();
	matrixL.push();
	matrixBig.push();
	matrixMid.push();
}

void MatrixManager::stopAnimation(){
	for(auto & anim : anims){
		delete anim;
		anim = nullptr;
	}
}

void MatrixManager::clear(bool on){
	auto color = on ? MatrixPixel::White : MatrixPixel::Off;
	matrixR.clear(color);
	matrixL.clear(color);
	matrixBig.clear(color);
	matrixMid.clear(color);

	if(on){
		push();
	}
}

void MatrixManager::playRandom(){
	if(unusedIdleAnimations.empty()){
		usedIdleAnimations.clear();
		for(uint8_t i = 1; i <= idleAnims; i++){
			unusedIdleAnimations.push_back(i);
		}
	}

	uint8_t index = random(unusedIdleAnimations.size());
	uint8_t animIndex = unusedIdleAnimations[index];
	unusedIdleAnimations.erase(unusedIdleAnimations.begin() + index);
	usedIdleAnimations.push_back(animIndex);

	char animPath[30];
	for(int i = 0; i < 4; i++){
		delete anims[i];
		sprintf(animPath, "/matrixGIF/%s%d.gif", PartitionNames[i], animIndex);
		anims[i] = new MatrixAnimGIF(SPIFFS.open(animPath));
		partitions[i]->startAnimation(anims[i]);
	}
}

void MatrixManager::startRandom(){
	if(playingRandom) return;

	usedIdleAnimations.clear();
	unusedIdleAnimations.clear();
	for(uint8_t i = 1; i <= idleAnims; i++){
		unusedIdleAnimations.push_back(i);
	}

	playRandom();

	playingRandom = true;
}

void MatrixManager::stopRandom(){
	stopAnimation();
	clear(false);
	push();

	playingRandom = false;
}

void MatrixManager::fillMatrixMid(uint16_t amp){
	matrixMid.clear();
	uint16_t total = ((float)amp / (float)255.0) * (float)(matrixMid.getWidth() * 255);
	uint16_t totalUpper = (float)(matrixMid.getWidth() * 255) - total;
	for(int i = 0; i < matrixMid.getWidth(); i++){
		if(totalUpper < i * 255){
			break;
		}else if(totalUpper > (i + 1) * 255){
			matrixMid.drawPixel(i, 0, {255, 255, 255, 255});
		}else{
			matrixMid.drawPixel(i, 0, {255, 255, 255, totalUpper - i * 255});
		}
	}
	for(int i = 0; i < matrixMid.getWidth(); i++){
		if(total < i * 255){
			break;
		}else if(total > (i + 1) * 255){
			matrixMid.drawPixel(matrixMid.getWidth() - 1 - i, 1, {255, 255, 255, 255});
		}else{
			matrixMid.drawPixel(matrixMid.getWidth() - 1 - i, 1, {255, 255, 255, total - i * 255});
		}
	}
}
