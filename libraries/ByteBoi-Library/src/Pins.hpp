#ifndef BYTEBOI_LIBRARY_PINS_HPP
#define BYTEBOI_LIBRARY_PINS_HPP

#include <cstdint>

enum class Pin : uint8_t {
	BtnUp, BtnDown, BtnLeft, BtnRight, BtnA, BtnB, BtnC,
	SpkrOut, SpkrEn, DetSD,
	Backlight,  LedR, LedG, LedB,
	AdcBatt, DetChrg, RefEn,
	SD_Sck, SD_Miso, SD_Mosi, SD_Cs,
	TFT_Sck, TFT_Mosi, TFT_Cs, TFT_Dc, TFT_Rst,
	ShiftScl, ShiftSda, ShiftPl,
	I2C_Sda, I2C_Scl
};

// This is the same on all versions, and is used in a lot of constexpr-cases - better to have them hard defined
#define BTN_UP 0
#define BTN_DOWN 3
#define BTN_LEFT 1
#define BTN_RIGHT 2
#define BTN_A 6
#define BTN_B 5
#define BTN_C 4

#define SPEAKER_PIN Pins.get(Pin::SpkrOut)
#define SPEAKER_SD Pins.get(Pin::SpkrEn)
#define BL_PIN Pins.get(Pin::Backlight)
#define BATTERY_PIN Pins.get(Pin::AdcBatt)

#define LED_R Pins.get(Pin::LedR)
#define LED_G Pins.get(Pin::LedG)
#define LED_B Pins.get(Pin::LedB)
#define CHARGE_DETECT_PIN Pins.get(Pin::DetChrg)
#define SD_DETECT_PIN Pins.get(Pin::DetSD)
#define TT1 &TomThumb

#define SPI_SCK Pins.get(Pin::SD_Sck)
#define SPI_MISO Pins.get(Pin::SD_Miso)
#define SPI_MOSI Pins.get(Pin::SD_Mosi)
#define SPI_SS -1
#define SD_CS Pins.get(Pin::SD_Cs)

#define I2C_SDA Pins.get(Pin::I2C_Sda)
#define I2C_SCL Pins.get(Pin::I2C_Scl)

#define SHIFT_SCL Pins.get(Pin::ShiftScl)
#define SHIFT_SDA Pins.get(Pin::ShiftSda)
#define SHIFT_PL Pins.get(Pin::ShiftPl)

#define CALIB_EN Pins.get(Pin::RefEn)


#endif //BYTEBOI_LIBRARY_PINS_HPP
