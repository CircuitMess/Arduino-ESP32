#ifndef SYNTHIA_LIBRARY_PINS_HPP
#define SYNTHIA_LIBRARY_PINS_HPP

#include <cstdint>

enum class Pin : uint8_t {
	I2C_Sda, I2C_Scl,
	I2C2_Sda, I2C2_Scl,
	Btn1, Btn2, Btn3, Btn4, Btn5, BtnEncL, BtnEncR,
	EncL1, EncL2, EncR1, EncR2,
	PotL, PotR,
	I2S_Bck, I2S_Ws, I2S_Do, I2S_Di,
	RGB_Clk, RGB_D1, RGB_D2, RGB_D3, RGB_D4,
	ShiftPl, ShiftClk, ShiftData,
	LED_B2, LED_B1, LED_R5, LED_R4, LED_R3, LED_R2, LED_R1, LED_G5, LED_B5, LED_B4, LED_B3, LED_G4, LED_G3, LED_G2, LED_G1,
	LED_G9, LED_R9, LED_B9, LED_G8, LED_R8, LED_B8, LED_G7, LED_R7, LED_G10, LED_R10, LED_B10, LED_B7, LED_G6, LED_R6, LED_B6
};

// Wire/I2C1 in HWv2
#define I2C_SDA Pins.get(Pin::I2C_Sda)
#define I2C_SCL Pins.get(Pin::I2C_Scl)

#define BTN_5 ((uint8_t) Pins.get(Pin::Btn5))
#define BTN_4 ((uint8_t) Pins.get(Pin::Btn4))
#define BTN_3 ((uint8_t) Pins.get(Pin::Btn3))
#define BTN_2 ((uint8_t) Pins.get(Pin::Btn2))
#define BTN_1 ((uint8_t) Pins.get(Pin::Btn1))
#define BTN_ENC_L ((uint8_t) Pins.get(Pin::BtnEncL))
#define BTN_ENC_R ((uint8_t) Pins.get(Pin::BtnEncR))

// Only present in HWv1
#define INP_PL Pins.get(Pin::ShiftPl)
#define INP_CLK Pins.get(Pin::ShiftClk)
#define INP_MISO Pins.get(Pin::ShiftData)

#define ENC_L1 Pins.get(Pin::EncL1)
#define ENC_L2 Pins.get(Pin::EncL2)

#define ENC_R1 Pins.get(Pin::EncR1)
#define ENC_R2 Pins.get(Pin::EncR2)

#define POT_L Pins.get(Pin::PotL)
#define POT_R Pins.get(Pin::PotR)

// Same on both HWv1 and HWv2
#define I2S_BCK 21
#define I2S_WS 4
#define I2S_DO 14
#define I2S_DI 15

#define RGB_CLK Pins.get(Pin::RGB_Clk)
#define RGB_DATA { 5, 2, 26, 13 } // Only present in HWv1

#endif //SYNTHIA_LIBRARY_PINS_HPP
