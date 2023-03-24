#include "BatControllerDisplay.h"
#include "Pins.hpp"
lgfx::Panel_ST7735S* BatControllerDisplay::panel1(){
	auto bus = new lgfx::Bus_SPI();
	auto panel = new lgfx::Panel_ST7735S();

	{
		auto cfg = bus->config();


		cfg.spi_host = VSPI_HOST;
		cfg.spi_mode = 0;
		cfg.freq_write = 27000000;
		cfg.freq_read = 27000000;
		cfg.spi_3wire = false;
		cfg.use_lock = true;
		cfg.dma_channel = 1;
		cfg.pin_sclk = TFT_SCL;
		cfg.pin_mosi = TFT_SDA;
		cfg.pin_miso = -1;
		cfg.pin_dc = TFT_DC;

		bus->config(cfg);
		panel->setBus(bus);
	}

	{
		auto cfg = panel->config();

		cfg.pin_cs = -1;
		cfg.pin_rst = TFT_RESET;
		cfg.pin_busy = -1;


		cfg.memory_width = 128;
		cfg.memory_height = 160;
		cfg.panel_width = 128;
		cfg.panel_height = 160;
		cfg.offset_x = 0;
		cfg.offset_y = 0;
		cfg.offset_rotation = 0;
		cfg.readable = false;
		cfg.invert = false;
		cfg.rgb_order = false;
		cfg.dlen_16bit = false;
		cfg.bus_shared = false;


		panel->config(cfg);
	}

	return panel;
}
