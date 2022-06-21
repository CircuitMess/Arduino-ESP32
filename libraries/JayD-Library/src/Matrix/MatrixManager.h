#ifndef JAYD_LIBRARY_MATRIXMANAGER_H
#define JAYD_LIBRARY_MATRIXMANAGER_H

#include <Arduino.h>
#include "MatrixBig.h"
#include "MatrixL.h"
#include "MatrixR.h"
#include "MatrixMid.h"
#include <Devices/Matrix/Matrix.h>
#include <Loop/LoopListener.h>

class MatrixAnimGIF;

class MatrixManager : public LoopListener {
public:
	MatrixManager(MatrixOutputBuffer* ledmatrix);

	void loop(uint time) override;
	void push();
	void stopAnimation();
	void clear(bool on = false);

	void startRandom();
	void stopRandom();

	void fillMatrixMid(uint16_t amp);

private:

	static const uint8_t idleAnims = 20;
	bool playingRandom = false;
	std::vector<uint> usedIdleAnimations;
	std::vector<uint> unusedIdleAnimations;
	void playRandom();

	MatrixBig matrixBigOut;
	MatrixL matrixLOut;
	MatrixR matrixROut;
	MatrixMid matrixMidOut;

	static const char* PartitionNames[4];
	Matrix* partitions[4];
	MatrixAnimGIF* anims[4] = {nullptr};

public:
	Matrix matrixBig;
	Matrix matrixL;
	Matrix matrixR;
	Matrix matrixMid;
};


#endif //JAYD_LIBRARY_MATRIXMANAGER_H
