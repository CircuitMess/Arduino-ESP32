#include "Compression.h"
#include "CompressionUtil.h"

Compression::Compression(int rate) :
		Compression(rate,
					0.000f,
					0.000f,
					-24.000f,
					30.000f,
					12.000f,
					0.003f,
					0.250f,
					0.006f,
					0.090f,
					0.160f,
					0.420f,
					0.980f,
					1.000f
		){

}

Compression::Compression(int rate, float pregain, float postgain, float threshold, float knee, float ratio, float attack, float release) :
		Compression(rate, pregain, postgain, threshold, knee, ratio, attack, release,
					0.006f,
					0.090f,
					0.160f,
					0.420f,
					0.980f,
					1.000f
		){
}

Compression::Compression(int rate, float pregain, float postgain, float threshold, float knee, float ratio, float attack, float release, float predelay, float releasezone1, float releasezone2, float releasezone3, float releasezone4, float wet){

	// setup the predelay buffer
	delaybufsize = rate * predelay;
	if (delaybufsize < 1)
		delaybufsize = 1;
	else if (delaybufsize > SF_COMPRESSOR_MAXDELAY)
		delaybufsize = SF_COMPRESSOR_MAXDELAY;

	delaybuf = static_cast<float*>(malloc(SF_COMPRESSOR_MAXDELAY * sizeof(float)));
	memset(delaybuf, 0, sizeof(float) * delaybufsize);

	// useful values
	linearpregain = db2lin(pregain);
	linearthreshold = db2lin(threshold);
	slope = 1.0f / ratio;
	float attacksamples = rate * attack;
	attacksamplesinv = 1.0f / attacksamples;
	float releasesamples = rate * release;
	float satrelease = 0.0025f; // seconds
	satreleasesamplesinv = 1.0f / ((float)rate * satrelease);
	dry = 1.0f - wet;

	// metering values (not used in core algorithm, but used to output a meter if desired)
	metergain = 1.0f; // gets overwritten immediately because gain will always be negative
	float meterfalloff = 0.325f; // seconds
	meterrelease = 1.0f - expf(-1.0f / ((float)rate * meterfalloff));

	// calculate knee curve parameters
	k = 5.0f; // initial guess
	kneedboffset = 0.0f;
	linearthresholdknee = 0.0f;
	if (knee > 0.0f){ // if a knee exists, search for a good k value
		float xknee = db2lin(threshold + knee);
		float mink = 0.1f;
		float maxk = 10000.0f;
		// search by comparing the knee slope at the current k guess, to the ideal slope
		for (int i = 0; i < 15; i++){
			if (kneeslope(xknee, k, linearthreshold) < slope)
				maxk = k;
			else
				mink = k;
			k = sqrtf(mink * maxk);
		}
		kneedboffset = lin2db(kneecurve(xknee, k, linearthreshold));
		linearthresholdknee = db2lin(threshold + knee);
	}

	// calculate a master gain based on what sounds good
	float fulllevel = compcurve(1.0f, k, slope, linearthreshold, linearthresholdknee,
								threshold, knee, kneedboffset);
	mastergain = db2lin(postgain) * powf(1.0f / fulllevel, 0.6f);

	// calculate the adaptive release curve parameters
	// solve a,b,c,d in `y = a*x^3 + b*x^2 + c*x + d`
	// interescting points (0, y1), (1, y2), (2, y3), (3, y4)
	float y1 = releasesamples * releasezone1;
	float y2 = releasesamples * releasezone2;
	float y3 = releasesamples * releasezone3;
	float y4 = releasesamples * releasezone4;
	a = (-y1 + 3.0f * y2 - 3.0f * y3 + y4) / 6.0f;
	b = y1 - 2.5f * y2 + 2.0f * y3 - 0.5f * y4;
	c = (-11.0f * y1 + 18.0f * y2 - 9.0f * y3 + 2.0f * y4) / 6.0f;
	d = y1;

	// save everything
	this->metergain            = 1.0f; // large value overwritten immediately since it's always < 0
	this->threshold            = threshold;
	this->knee                 = knee;
	this->wet                  = wet;
	this->detectoravg          = 0.0f;
	this->compgain             = 1.0f;
	this->maxcompdiffdb        = -1.0f;
	this->delaywritepos        = 0;
	this->delayreadpos         = delaybufsize > 1 ? 1 : 0;
}

Compression::~Compression(){
	delete delaybuf;
}

float Compression::getMeter(){
	return metergain;
}

void Compression::process(int16_t* inputBuffer, int16_t* outputBuffer, size_t noSamples){
	int samplesperchunk = SF_COMPRESSOR_SPU;
	int chunks = noSamples / samplesperchunk;
	float ang90 = (float)M_PI * 0.5f;
	float ang90inv = 2.0f / (float)M_PI;
	int samplepos = 0;
	float spacingdb = SF_COMPRESSOR_SPACINGDB;

	for (int ch = 0; ch < chunks; ch++){
		detectoravg = fixf(detectoravg, 1.0f);
		float desiredgain = detectoravg;
		float scaleddesiredgain = asinf(desiredgain) * ang90inv;
		float compdiffdb = lin2db(compgain / scaleddesiredgain);

		// calculate envelope rate based on whether we're attacking or releasing
		float enveloperate;
		if (compdiffdb < 0.0f){ // compgain < scaleddesiredgain, so we're releasing
			compdiffdb = fixf(compdiffdb, -1.0f);
			maxcompdiffdb = -1; // reset for a future attack mode
			// apply the adaptive release curve
			// scale compdiffdb between 0-3
			float x = (clampf(compdiffdb, -12.0f, 0.0f) + 12.0f) * 0.25f;
			float releasesamples = adaptivereleasecurve(x, a, b, c, d);
			enveloperate = db2lin(spacingdb / releasesamples);
		}
		else{ // compresorgain > scaleddesiredgain, so we're attacking
			compdiffdb = fixf(compdiffdb, 1.0f);
			if (maxcompdiffdb == -1 || maxcompdiffdb < compdiffdb)
				maxcompdiffdb = compdiffdb;
			float attenuate = maxcompdiffdb;
			if (attenuate < 0.5f)
				attenuate = 0.5f;
			enveloperate = 1.0f - powf(0.25f / attenuate, attacksamplesinv);
		}

		// process the chunk
		for (int chi = 0; chi < samplesperchunk; chi++, samplepos++,
				delayreadpos = (delayreadpos + 1) % delaybufsize,
				delaywritepos = (delaywritepos + 1) % delaybufsize){

			float input = ((float) inputBuffer[samplepos] / SAMPLE_MAX_AMP) * linearpregain;
			delaybuf[delaywritepos] = input;

			input = absf(input);
			// float inputmax = inputL > inputR ? inputL : inputR;

			float attenuation;
			if (input < 0.0001f)
				attenuation = 1.0f;
			else{
				float inputcomp = compcurve(input, k, slope, linearthreshold, linearthresholdknee, threshold, knee, kneedboffset);
				attenuation = inputcomp / input;
			}

			float rate;
			if (attenuation > detectoravg){ // if releasing
				float attenuationdb = -lin2db(attenuation);
				if (attenuationdb < 2.0f)
					attenuationdb = 2.0f;
				float dbpersample = attenuationdb * satreleasesamplesinv;
				rate = db2lin(dbpersample) - 1.0f;
			}
			else
				rate = 1.0f;

			detectoravg += (attenuation - detectoravg) * rate;
			if (detectoravg > 1.0f)
				detectoravg = 1.0f;
			detectoravg = fixf(detectoravg, 1.0f);

			if (enveloperate < 1) // attack, reduce gain
				compgain += (scaleddesiredgain - compgain) * enveloperate;
			else{ // release, increase gain
				compgain *= enveloperate;
				if (compgain > 1.0f)
					compgain = 1.0f;
			}

			// the final gain value!
			float premixgain = sinf(ang90 * compgain);
			float gain = dry + wet * mastergain * premixgain;

			// calculate metering (not used in core algo, but used to output a meter if desired)
			float premixgaindb = lin2db(premixgain);
			if (premixgaindb < metergain)
				metergain = premixgaindb; // spike immediately
			else
				metergain += (premixgaindb - metergain) * meterrelease; // fall slowly

			// apply the gain
			outputBuffer[samplepos] = delaybuf[delayreadpos] * (SAMPLE_MAX_AMP - 2.0) * gain;
		}
	}
}
