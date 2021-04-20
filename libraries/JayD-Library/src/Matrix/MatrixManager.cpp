#include <Loop/LoopManager.h>
#include <SPIFFS.h>
#include "MatrixManager.h"

const char* MatrixManager::PartitionNames[] = { "left", "right", "mid", "big" };

MatrixManager::MatrixManager(LEDmatrixImpl* ledmatrix) : ledmatrix(ledmatrix), matrixBig(ledmatrix),
														 matrixL(ledmatrix), matrixR(ledmatrix), matrixMid(ledmatrix){

	partitions[0] = &matrixL;
	partitions[1] = &matrixR;
	partitions[2] = &matrixMid;
	partitions[3] = &matrixBig;
}

void MatrixManager::loop(uint time){
	matrixR.loop(time);
	matrixL.loop(time);
	matrixBig.loop(time);
	matrixMid.loop(time);
	ledmatrix->loop(time);

	if(playingRandom && partitions[0]->getAnimationCompletionRate() >= 99.0f){
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
	matrixR.stopAnimation();
	matrixL.stopAnimation();
	matrixBig.stopAnimation();
	matrixMid.stopAnimation();
}

void MatrixManager::clear(bool on){
	matrixR.clear(on);
	matrixL.clear(on);
	matrixBig.clear(on);
	matrixMid.clear(on);
	if(on){
		push();
		return;
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
		sprintf(animPath, "/matrixGIF/%s%d.gif", PartitionNames[i], animIndex);
		partitions[i]->startAnimation(new Animation(new File(SPIFFS.open(animPath))), true);
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
