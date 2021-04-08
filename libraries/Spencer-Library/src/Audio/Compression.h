// Based on sndfilter by Sean Connely, https://github.com/voidqk/sndfilter
// MIT License

// dynamics compressor based on WebAudio specification: https://webaudio.github.io/web-audio-api/#the-dynamicscompressornode-interface

#ifndef SPENCER_COMPRESSION_H
#define SPENCER_COMPRESSION_H

#include <Arduino.h>

// maximum number of samples in the delay buffer
#define SF_COMPRESSOR_MAXDELAY   1024

// samples per update; the compressor works by dividing the input chunks into even smaller sizes,
// and performs heavier calculations after each mini-chunk to adjust the final envelope
#define SF_COMPRESSOR_SPU        32

// not sure what this does exactly, but it is part of the release curve
#define SF_COMPRESSOR_SPACINGDB  5.0f

#define SAMPLE_MAX_AMP 32768.0

class Compression {
public:
	/**
	 * Default
	 * @param rate
	 */
	explicit Compression(int rate);

	/**
	 * Simple
	 * @param rate Input sample rate (samples per second)
	 * @param pregain dB, amount to boost the signal before applying compression [0 to 100]
	 * @param postgain dB, amount to boost the signal after applying compression [0 to 100]
	 * @param threshold dB, level where compression kicks in [-100 to 0]
	 * @param knee dB, width of the knee [0 to 40]
	 * @param ratio unitless, amount to inversely scale the output when applying comp [1 to 20]
	 * @param attack seconds, length of the attack phase [0 to 1]
	 * @param release seconds, length of the release phase [0 to 1]
	 */
	Compression(int rate, float pregain, float postgain, float threshold, float knee, float ratio, float attack, float release);

	/**
	 * Advanced
	 * @param rate Input sample rate (samples per second)
	 * @param pregain dB, amount to boost the signal before applying compression [0 to 100]
	 * @param threshold dB, level where compression kicks in [-100 to 0]
	 * @param knee dB, width of the knee [0 to 40]
	 * @param ratio unitless, amount to inversely scale the output when applying comp [1 to 20]
	 * @param attack seconds, length of the attack phase [0 to 1]
	 * @param release seconds, length of the release phase [0 to 1]
	 * @param predelay seconds, length of the predelay buffer [0 to 1]
	 * @param releasezone1 release zones should be increasing between 0 and 1, and are a fraction
	 * @param releasezone2  of the release time depending on the input dB -- these parameters define
	 * @param releasezone3  the adaptive release curve, which is discussed in further detail in the
	 * @param releasezone4  demo: adaptive-release-curve.html
	 * @param postgain dB, amount of gain to apply after compression [0 to 100]
	 * @param wet amount to apply the effect [0 completely dry to 1 completely wet]
	 */
	Compression(int rate, float pregain, float threshold, float knee, float ratio, float attack, float release, float predelay, float releasezone1, float releasezone2, float releasezone3, float releasezone4, float postgain, float wet);

	virtual ~Compression();

	void process(int16_t* input, int16_t* output, size_t noSamples);

	float getMeter();

private:
	// user can read the metergain state variable after processing a chunk to see how much dB the
	// compressor would have liked to compress the sample; the meter values aren't used to shape the
	// sound in any way, only used for output if desired
	float metergain;

	// everything else shouldn't really be mucked with unless you read the algorithm and feel
	// comfortable
	float meterrelease;
	float threshold;
	float knee;
	float linearpregain;
	float linearthreshold;
	float slope;
	float attacksamplesinv;
	float satreleasesamplesinv;
	float wet;
	float dry;
	float k;
	float kneedboffset;
	float linearthresholdknee;
	float mastergain;
	float a; // adaptive release polynomial coefficients
	float b;
	float c;
	float d;
	float detectoravg;
	float compgain;
	float maxcompdiffdb;
	int delaybufsize;
	int delaywritepos;
	int delayreadpos;
	float* delaybuf; // predelay buffer
};

#endif // SPENCER_COMPRESSION_H
