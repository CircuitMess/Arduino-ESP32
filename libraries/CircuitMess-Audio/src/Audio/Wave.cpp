#include "Wave.h"
#include "../Setup.hpp"

double Wave::fast_sin(double x) //From Orson Peters, github
{
	const double INVPI =  0.31830988618379067153776752674502872;
	const double A	 =  0.00735246819687011731341356165096815;
	const double B	 = -0.16528911397014738207016302002888890;
	const double C	 =  0.99969198629596757779830113868360584;
	int32_t k;
	double x2;
	k = round(INVPI * x);
	x -= k * PI;
	x2 = x * x;
	x = x*(C + x2*(B + A*x2));
	if (k % 2) x = -x;
	return x;
}


double (*Wave::generator[Type::COUNT])(uint16_t freq, size_t index) = {
		[](uint16_t freq, size_t index) -> double { return fast_sin(2.0 * M_PI * (double) freq * ((double) index / (double) SAMPLE_RATE)); },
		[](uint16_t freq, size_t index) -> double { return ((double) freq * ((double) index / (double) SAMPLE_RATE)) * 2.0 - 1.0; },
		[](uint16_t freq, size_t index) -> double { return (((double) freq * ((double) index / (double) SAMPLE_RATE)) < 0.5 ? 1.0 : 0) * 2.0 - 1.0; }
};

Wave::Wave(uint16_t freq, uint16_t duration, Wave::Type type) : freq(freq), duration(duration), type(type){
	float T = 1.0f / (float) freq;
	sampleCount = round(T * (float) SAMPLE_RATE);
	count = (size_t) (((float) duration / 1000.0f) * (float) SAMPLE_RATE);
}

size_t Wave::generate(int16_t* outBuffer){
	if(current == count && duration != 0) return 0;

	size_t samples = duration == 0 ? BUFFER_SAMPLES : min((size_t) BUFFER_SAMPLES, (count - current));

	for(int i = 0; i < samples; i++){
		outBuffer[i] = (int16_t) (generator[type](freq, sampleIndex) * 32767.0);
		current = min(count, current + 1);
		sampleIndex = (sampleIndex + 1) % sampleCount;
	}

	return samples;
}

int Wave::available(){
	if(duration == 0) return BUFFER_SAMPLES;
	else return (int) (count - current);
}

void Wave::end(){
	done = true;
}

bool Wave::isDone() const{
	return done;
}
