#include "CompressionUtil.h"
#include <math.h>

float db2lin(float db){ // dB to linear
	return powf(10.0f, 0.05f * db);
}

float lin2db(float lin){ // linear to dB
	return 20.0f * log10f(lin);
}

// for more information on the knee curve, check out the compressor-curve.html demo + source code
// included in this repo
float kneecurve(float x, float k, float linearthreshold){
	return linearthreshold + (1.0f - expf(-k * (x - linearthreshold))) / k;
}

float kneeslope(float x, float k, float linearthreshold){
	return k * x / ((k * linearthreshold + 1.0f) * expf(k * (x - linearthreshold)) - 1);
}

float compcurve(float x, float k, float slope, float linearthreshold,
							  float linearthresholdknee, float threshold, float knee, float kneedboffset){
	if (x < linearthreshold)
		return x;
	if (knee <= 0.0f) // no knee in curve
		return db2lin(threshold + slope * (lin2db(x) - threshold));
	if (x < linearthresholdknee)
		return kneecurve(x, k, linearthreshold);
	return db2lin(kneedboffset + slope * (lin2db(x) - threshold - knee));
}

float adaptivereleasecurve(float x, float a, float b, float c, float d){
	// a*x^3 + b*x^2 + c*x + d
	float x2 = x * x;
	return a * x2 * x + b * x2 + c * x + d;
}

float clampf(float v, float min, float max){
	return v < min ? min : (v > max ? max : v);
}

float absf(float v){
	return v < 0.0f ? -v : v;
}

float fixf(float v, float def){
	// fix NaN and infinity values that sneak in... not sure why this is needed, but it is
	if (isnan(v) || isinf(v))
		return def;
	return v;
}