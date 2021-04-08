#include "I2S.h"
#define SAMPLE_RATE (16000)
#define PIN_I2S_BCLK 16
#define PIN_I2S_LRC 27
#define PIN_I2S_DIN 32
#define PIN_I2S_DOUT 4

I2S::I2S() {}


// This I2S specification : 
//  -   LRC high is channel 2 (right).
//  -   LRC signal transitions once each word.
//  -   DATA is valid on the CLOCK rising edge.
//  -   Data bits are MSB first.
//  -   DATA bits are left-aligned with respect to LRC edge.
//  -   DATA bits are right-shifted by one with respect to LRC edges.
void I2S::begin() {
	inited = 1;
	// Serial.println("Configuring I2S...");
	esp_err_t err;

	// The I2S config as per the example
	const i2s_config_t i2s_config = {
		.mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_TX), // Receive, not transfer
		.sample_rate = SAMPLE_RATE,                         // 16KHz
		.bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT, // could only get it to work with 32bits
		.channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT, // although the SEL config should be left, it seems to transmit on right
		.communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
		.intr_alloc_flags = 0,     // Interrupt level 1
		.dma_buf_count = 16,                           // number of buffers
		.dma_buf_len = 60,                              // 60 samples per buffer (8 minimum)
		.use_apll = false
	};

	// The pin config as per the setup
	const i2s_pin_config_t pin_config = {
		.bck_io_num = PIN_I2S_BCLK,   // BCKL
		.ws_io_num = PIN_I2S_LRC,    // LRCL
		.data_out_num = PIN_I2S_DOUT, // not used (only for speakers)
		.data_in_num = PIN_I2S_DIN
	};

	// Configuring the I2S driver and pins.
	// This function must be called before any I2S driver read/write operations.
	err = i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
	if (err != ESP_OK) {
		Serial.printf("Failed installing I2S driver: %d\n", err);
		while (true);
	}
	err = i2s_set_pin(I2S_NUM_0, &pin_config);
	if (err != ESP_OK) {
		Serial.printf("Failed setting I2S pin: %d\n", err);
		while (true);
	}
	// Serial.println("I2S driver installed.");
}

int I2S::Read(char* data, int numData) {
	size_t readSize;
	i2s_read(I2S_NUM_0, (char *)data, numData, &readSize, portMAX_DELAY);
	return readSize;
}

void I2S::stop()
{
	inited = 0;
	i2s_driver_uninstall(I2S_NUM_0);
}

bool I2S::isInited()
{
	return inited;
}