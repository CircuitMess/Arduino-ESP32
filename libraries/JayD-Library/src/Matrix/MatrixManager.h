#ifndef JAYD_LIBRARY_MATRIXMANAGER_H
#define JAYD_LIBRARY_MATRIXMANAGER_H

#include <Arduino.h>
#include "MatrixBig.h"
#include "MatrixL.h"
#include "MatrixR.h"
#include "MatrixMid.h"

class MatrixManager : public LoopListener {
public:
	MatrixManager(LEDmatrixImpl* ledmatrix);
	MatrixBig matrixBig;
	MatrixL matrixL;
	MatrixR matrixR;
	MatrixMid matrixMid;

	void loop(uint time);
	void push();
	void stopAnimation();
	void clear(bool on = false);

	void startRandom();
	void stopRandom();

private:
	LEDmatrixImpl* ledmatrix;

	static const uint8_t idleAnims = 20;
	bool playingRandom = false;
	std::vector<uint> usedIdleAnimations;
	std::vector<uint> unusedIdleAnimations;
	void playRandom();

	static const char* PartitionNames[4];
	MatrixPartition* partitions[4];
};


#endif //JAYD_LIBRARY_MATRIXMANAGER_H
