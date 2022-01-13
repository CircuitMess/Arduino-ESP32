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

#define BUFFER_SIZE	(BUFFER_SAMPLES * BYTES_PER_SAMPLE * NUM_CHANNELS)

// DAC output settings

#ifndef DAC_TIMER
#define DAC_TIMER 1
#endif

#ifndef DAC_BUFFERS
#define DAC_BUFFERS 6
#endif


// AAC decoder settings

#ifndef AAC_READ_BUFFER
#define AAC_READ_BUFFER 1024 * 8 // Read buffer size
#endif

#ifndef AAC_READ_CHUNK
#define AAC_READ_CHUNK 1024 * 2 // Read chunk size, should be bigger than min. decoder input
#endif

#ifndef AAC_DECODE_MIN_INPUT
#define AAC_DECODE_MIN_INPUT 1024 // Min. decoder input in
#endif

#ifndef AAC_OUT_BUFFER
#define AAC_OUT_BUFFER 1024 * 4 // Decoder output buffer size
#endif