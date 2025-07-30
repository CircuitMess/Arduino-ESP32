#include "ByteBoiDisplay.h"
#include "ByteBoi.h"

lgfx::Panel_ILI9341* ByteBoiDisplay::panel1(){
	auto bus = new lgfx::Bus_SPI();
	auto panel = new lgfx::Panel_ILI9341();

	{
		auto cfg = bus->config();

		cfg.spi_host = VSPI_HOST;
		cfg.spi_mode = 0;
		cfg.freq_write = 60000000;
		cfg.freq_read = 60000000;
		cfg.spi_3wire = false;
		cfg.use_lock = true;
		cfg.dma_channel = 1;
		cfg.pin_sclk = Pins.get(Pin::SD_Sck);
		cfg.pin_mosi = Pins.get(Pin::SD_Mosi);
		cfg.pin_miso = Pins.get(Pin::SD_Miso);
		cfg.pin_dc = Pins.get(Pin::TFT_Dc);

		bus->config(cfg);
		panel->setBus(bus);
	}

	{
		auto cfg = panel->config();

		cfg.pin_cs = Pins.get(Pin::TFT_Cs);
		cfg.pin_rst = Pins.get(Pin::TFT_Rst);
		cfg.pin_busy = -1;

		cfg.memory_width = 240;
		cfg.memory_height = 320;
		cfg.panel_width = 240;
		cfg.panel_height = 320;
		cfg.offset_x = 0;
		cfg.offset_y = 0;
		cfg.offset_rotation = 0;
		cfg.readable = true;
		cfg.invert = false;
		cfg.rgb_order = false;
		cfg.dlen_16bit = false;
		cfg.bus_shared = true;

		panel->config(cfg);
	}

	return panel;
}

lgfx::Panel_ST7789* ByteBoiDisplay::panel2(){
	auto bus = new lgfx::Bus_SPI();
	auto panel = new lgfx::Panel_ST7789();

	{
		auto cfg = bus->config();

		cfg.spi_host = HSPI_HOST;
		cfg.spi_mode = 3;
		cfg.freq_write = 60000000;
		cfg.freq_read = 60000000;
		cfg.spi_3wire = false;
		cfg.use_lock = true;
		cfg.dma_channel = 1;
		cfg.pin_sclk = Pins.get(Pin::TFT_Sck);
		cfg.pin_mosi = Pins.get(Pin::TFT_Mosi);
		cfg.pin_miso = -1;
		cfg.pin_dc = Pins.get(Pin::TFT_Dc);

		bus->config(cfg);
		panel->setBus(bus);
	}

	{
		auto cfg = panel->config();

		cfg.pin_cs = -1;
		cfg.pin_rst = Pins.get(Pin::TFT_Rst);
		cfg.pin_busy = -1;

		cfg.memory_width = 240;
		cfg.memory_height = 320;
		cfg.panel_width = 240;
		cfg.panel_height = 320;
		cfg.offset_x = 0;
		cfg.offset_y = 0;
		cfg.offset_rotation = 2;
		cfg.readable = true;
		cfg.invert = false;
		cfg.rgb_order = false;
		cfg.dlen_16bit = false;
		cfg.bus_shared = true;

		panel->config(cfg);
	}

	return panel;
}

lgfx::Panel_ST7789* ByteBoiDisplay::panel3(){
	auto bus = new lgfx::Bus_SPI();
	auto panel = new lgfx::Panel_ST7789();

	{
		auto cfg = bus->config();

		cfg.spi_host = HSPI_HOST;
		cfg.spi_mode = 3;
		cfg.freq_write = 60000000;
		cfg.freq_read = 60000000;
		cfg.spi_3wire = false;
		cfg.use_lock = true;
		cfg.dma_channel = 1;
		cfg.pin_sclk = Pins.get(Pin::TFT_Sck);
		cfg.pin_mosi = Pins.get(Pin::TFT_Mosi);
		cfg.pin_miso = -1;
		cfg.pin_dc = Pins.get(Pin::TFT_Dc);

		bus->config(cfg);
		panel->setBus(bus);
	}

	{
		auto cfg = panel->config();

		cfg.pin_cs = -1;
		cfg.pin_rst = Pins.get(Pin::TFT_Rst);
		cfg.pin_busy = -1;

		cfg.memory_width = 240;
		cfg.memory_height = 320;
		cfg.panel_width = 240;
		cfg.panel_height = 320;
		cfg.offset_x = 0;
		cfg.offset_y = 0;
		cfg.offset_rotation = 0;
		cfg.readable = true;
		cfg.invert = false;
		cfg.rgb_order = false;
		cfg.dlen_16bit = false;
		cfg.bus_shared = true;

		panel->config(cfg);
	}

	return panel;
}
