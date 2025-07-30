#include "PinDef.h"

const PinDefMap<Pin> Pins1 = {
		{ Pin::I2C_Sda,   23 },
		{ Pin::I2C_Scl,   22 },

		{ Pin::Btn5,      6 },
		{ Pin::Btn4,      3 },
		{ Pin::Btn3,      2 },
		{ Pin::Btn2,      1 },
		{ Pin::Btn1,      0 },
		{ Pin::BtnEncL,   5 },
		{ Pin::BtnEncR,   4 },

		{ Pin::ShiftPl,   19 },
		{ Pin::ShiftClk,  25 },
		{ Pin::ShiftData, 27 },

		{ Pin::EncL1,     34 },
		{ Pin::EncL2,     35 },
		{ Pin::EncR1,     32 },
		{ Pin::EncR2,     33 },

		{ Pin::PotL,      39 },
		{ Pin::PotR,      36 },

		{ Pin::I2S_Bck,   21 },
		{ Pin::I2S_Ws,    4 },
		{ Pin::I2S_Do,    14 },
		{ Pin::I2S_Di,    15 },

		{ Pin::RGB_Clk,   12 },
		{ Pin::RGB_D1,    5 },
		{ Pin::RGB_D2,    2 },
		{ Pin::RGB_D3,    26 },
		{ Pin::RGB_D4,    13 },
};

const PinDefMap<Pin> Pins2 = {
		{ Pin::I2C_Sda,  22 },
		{ Pin::I2C_Scl,  23 },
		{ Pin::I2C2_Sda, 5 },
		{ Pin::I2C2_Scl, 18 },

		{ Pin::Btn5,     4 },
		{ Pin::Btn4,     3 },
		{ Pin::Btn3,     2 },
		{ Pin::Btn2,     1 },
		{ Pin::Btn1,     0 },
		{ Pin::BtnEncL,  15 },
		{ Pin::BtnEncR,  14 },

		{ Pin::EncL1,    25 },
		{ Pin::EncL2,    26 },
		{ Pin::EncR1,    32 },
		{ Pin::EncR2,    33 },

		{ Pin::PotL,     39 },
		{ Pin::PotR,     36 },

		{ Pin::I2S_Bck,  21 },
		{ Pin::I2S_Ws,   4 },
		{ Pin::I2S_Do,   14 },
		{ Pin::I2S_Di,   15 },

		//Schematic footprints had errors in v2.6 which swapped G and B channels for following LEDs:
		// First AW9523 expander (track RGBs)
		{ Pin::LED_B2,   0 },
		{ Pin::LED_B1,   1 },
		{ Pin::LED_R5,   2 },
		{ Pin::LED_R4,   3 },
		{ Pin::LED_R3,   4 },
		{ Pin::LED_R2,   5 },
		{ Pin::LED_R1,   6 },
		{ Pin::LED_G5,   7 },
		{ Pin::LED_B5,   9 },
		{ Pin::LED_B4,   10 },
		{ Pin::LED_B3,   11 },
		{ Pin::LED_G4,   12 },
		{ Pin::LED_G3,   13 },
		{ Pin::LED_G2,   14 },
		{ Pin::LED_G1,   15 },

		// Second AW9523 expander (slot RGBs)
		{ Pin::LED_G9,   0 },
		{ Pin::LED_R9,   1 },
		{ Pin::LED_B9,   2 },
		{ Pin::LED_G8,   3 },
		{ Pin::LED_R8,   4 },
		{ Pin::LED_B8,   5 },
		{ Pin::LED_G7,   6 },
		{ Pin::LED_R7,   7 },
		{ Pin::LED_G10,  9 },
		{ Pin::LED_R10,  10 },
		{ Pin::LED_B10,  11 },
		{ Pin::LED_B7,   12 },
		{ Pin::LED_G6,   13 },
		{ Pin::LED_R6,   14 },
		{ Pin::LED_B6,   15 },
};
