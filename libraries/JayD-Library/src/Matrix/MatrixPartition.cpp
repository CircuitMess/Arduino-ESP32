#include "MatrixPartition.h"

MatrixPartition::MatrixPartition(LEDmatrixImpl* matrix, uint8_t width, uint8_t height) : matrix(matrix), width(width), height(height){
	buffer = static_cast<uint8_t*>(malloc(width * height));
}

void MatrixPartition::drawPixel(int x, int y, uint8_t brightness){
	buffer[y * width + x] = brightness;
}

void MatrixPartition::clear(bool on){
	if(on){
		memset(buffer, 255, width * height);
	}else{
		memset(buffer, 0, width * height);
	}
}

void MatrixPartition::vu(uint16_t amp){
	clear();
	uint16_t total = ((float) amp / (float)INT16_MAX) * (float) (height*255);
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			if(total < i*255) {
				break;
			}else if(total > (i+1)*255) {
				drawPixel(j, height - 1 - i, 255);
			}else{
				drawPixel(j, height - 1 - i, total - i*255);
			}
		}
	}
}

void MatrixPartition::drawBitmap(int x, int y, uint width, uint height, uint8_t* data)
{
	for(int i = 0; i < height; i++)
	{
		for(int j = 0; j < width; j++)
		{
			drawPixel(x + j, y + i, data[i*width + j]);
		}
	}
}

void MatrixPartition::startAnimation(Animation* _animation, bool loop)
{
	stopAnimation();

	animation = _animation;
	animationLoop = loop;
	animationFrame = animation->getNextFrame();
	drawBitmap(0, 0, animation->getWidth(), animation->getHeight(), animationFrame->data);
	push();
	currentFrameTime = 0;
	animationStartMicros = micros();
}

/**************************************************************************/
/*!
	@brief  Stops running animation.
*/
/**************************************************************************/
void MatrixPartition::stopAnimation()
{
	delete animation;
	animation = nullptr;
	animationFrame = nullptr;
	currentFrameTime = 0;
}

/**************************************************************************/
/*!
	@brief  Updates running animation. Expected to be used inside loop().
	@param  _time Current millis() time, used for frame duration calculation.
*/
/**************************************************************************/
void MatrixPartition::loop(uint _time)
{
	if(animationFrame != nullptr && animation != nullptr){
		currentFrameTime+=_time;
		if(currentFrameTime >= animationFrame->duration*1000){
			clear();
			currentFrameTime = 0;
			animationFrame = animation->getNextFrame();
			if(animationFrame == nullptr){
				if(animationLoop){
					animationStartMicros = micros();
					animation->rewind();
					animationFrame = animation->getNextFrame();
				}else{
					stopAnimation();
					return;
				}
			}
			drawBitmap(0, 0, animation->getWidth(), animation->getHeight(), animationFrame->data);
			push();
			return;
		}
	}
}

/**************************************************************************/
/*!
	@brief  Returns the completion rate in percentage for the current animation.
	@return  Completion rate (in percentage 0-100) for the current animation. If none are played, then defaults to zero.
*/
/**************************************************************************/
float MatrixPartition::getAnimationCompletionRate()
{
	if(animationFrame == nullptr || animation == nullptr) return 0.0;
	return ((float)(micros() - animationStartMicros)) / ((float)(animation->getLoopDuration()*1000))*100;
}