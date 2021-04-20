/*
These settings are usually defined in the board definitions file,
but if you're compiling for a board that doesn't have them defined, you can do so here.
*/

#ifndef BUFFER_SAMPLES
#define BUFFER_SAMPLES 256 //number of samples in the buffer
#endif

#ifndef SAMPLE_RATE
#define SAMPLE_RATE 24000
#endif

#ifndef BYTES_PER_SAMPLE
#define BYTES_PER_SAMPLE 2
#endif

#ifndef NUM_CHANNELS
#define NUM_CHANNELS 1
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE	(BUFFER_SAMPLES * BYTES_PER_SAMPLE * NUM_CHANNELS)
#endif
