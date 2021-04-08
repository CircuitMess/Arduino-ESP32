#ifndef SPENCER_COMPRESSIONUTIL_H
#define SPENCER_COMPRESSIONUTIL_H


float db2lin(float db);
float lin2db(float lin);
float kneecurve(float x, float k, float linearthreshold);
float kneeslope(float x, float k, float linearthreshold);
float compcurve(float x, float k, float slope, float linearthreshold, float linearthresholdknee, float threshold, float knee, float kneedboffset);
float adaptivereleasecurve(float x, float a, float b, float c, float d);
float clampf(float v, float min, float max);
float absf(float v);
float fixf(float v, float def);


#endif //SPENCER_COMPRESSIONUTIL_H
