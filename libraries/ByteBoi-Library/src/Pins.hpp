#ifndef BYTEBOI_LIBRARY_PINS_HPP
#define BYTEBOI_LIBRARY_PINS_HPP

#define BTN_UP 0
#define BTN_DOWN 3
#define BTN_LEFT 1
#define BTN_RIGHT 2
#define BTN_A 6
#define BTN_B 5
#define BTN_C 4

#define SPEAKER_PIN 25
#define SPEAKER_SD 13
#define BL_PIN (ByteBoi.getPin(0))
#define BATTERY_PIN 36

#define LED_R (ByteBoi.getPin(1))
#define LED_G (ByteBoi.getPin(2))
#define LED_B (ByteBoi.getPin(3))
#define CHARGE_DETECT_PIN (ByteBoi.getPin(4))
#define SD_DETECT_PIN (ByteBoi.getPin(5))
#define TT1 &TomThumb

#define SPI_SCK (ByteBoi.getPin(6))
#define SPI_MISO 5
#define SPI_MOSI 32
#define SPI_SS -1
#define SD_CS 2

#define I2C_SDA 23
#define I2C_SCL 22
#define I2C_EXPANDER_ADDR 0x74

#define SHIFT_SCL 15
#define SHIFT_SDA 4
#define SHIFT_PL 0


#endif //BYTEBOI_LIBRARY_PINS_HPP
