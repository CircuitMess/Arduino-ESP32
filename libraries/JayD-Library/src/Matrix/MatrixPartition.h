#ifndef JAYD_LIBRARY_MATRIXPARTITION_H
#define JAYD_LIBRARY_MATRIXPARTITION_H

#include <Arduino.h>
#include <Devices/LEDmatrix/LEDmatrix.h>

class MatrixPartition : public LoopListener {
public:
	MatrixPartition(LEDmatrixImpl* matrix, uint8_t width, uint8_t height);

	virtual void vu(uint16_t amp);
	void drawPixel(int x, int y, uint8_t brightness);
	void clear(bool on = false);

	void startAnimation(Animation* _animation, bool loop);
	void stopAnimation();
	float getAnimationCompletionRate();
	void loop(uint _time) override;
	void drawBitmap(int x, int y, uint width, uint height, uint8_t* data);

	virtual void push() = 0;
protected:
	LEDmatrixImpl* matrix;

	uint8_t width;
	uint8_t height;

	uint8_t* buffer;

	AnimationFrame* animationFrame = nullptr;
	Animation* animation = nullptr;
	bool animationLoop = 0;
	uint currentFrameTime = 0;
	uint animationStartMicros = 0;

};


#endif //JAYD_LIBRARY_MATRIXPARTITION_H
