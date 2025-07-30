#include "PinDef.h"

const PinDefMap<Pin> Pins1 = {
		{ Pin::BtnUp,     0 },
		{ Pin::BtnDown,   3 },
		{ Pin::BtnLeft,   1 },
		{ Pin::BtnRight,  2 },
		{ Pin::BtnA,      6 },
		{ Pin::BtnB,      5 },
		{ Pin::BtnC,      4 },

		{ Pin::SpkrOut,   25 },
		{ Pin::SpkrEn,    13 },
		{ Pin::Backlight, 12 },
		{ Pin::AdcBatt,   36 },

		{ Pin::LedR,      14 },
		{ Pin::LedG,      15 },
		{ Pin::LedB,      13 },
		{ Pin::DetChrg,   10 },
		{ Pin::DetSD,     8 },

		{ Pin::SD_Sck,    26 },
		{ Pin::SD_Miso,   5 },
		{ Pin::SD_Mosi,   32 },
		{ Pin::SD_Cs,     2 },

		{ Pin::TFT_Dc,    21 },
		{ Pin::TFT_Cs,    33 },
		{ Pin::TFT_Rst,   27 },

		{ Pin::I2C_Sda,   23 },
		{ Pin::I2C_Sda,   22 },
};

const PinDefMap<Pin> Pins2 = {
		{ Pin::BtnUp,     0 },
		{ Pin::BtnDown,   3 },
		{ Pin::BtnLeft,   1 },
		{ Pin::BtnRight,  2 },
		{ Pin::BtnA,      6 },
		{ Pin::BtnB,      5 },
		{ Pin::BtnC,      4 },

		{ Pin::SpkrOut,   25 },
		{ Pin::SpkrEn,    13 },
		{ Pin::Backlight, 18 },
		{ Pin::AdcBatt,   36 },

		{ Pin::LedR,      22 },
		{ Pin::LedG,      23 },
		{ Pin::LedB,      19 },
		{ Pin::DetChrg,   35 },
		{ Pin::DetSD,     7 },

		{ Pin::SD_Sck,    14 },
		{ Pin::SD_Miso,   5 },
		{ Pin::SD_Mosi,   32 },
		{ Pin::SD_Cs,     2 },

		{ Pin::TFT_Sck,   26 },
		{ Pin::TFT_Mosi,  33 },
		{ Pin::TFT_Dc,    21 },
		{ Pin::TFT_Rst,   27 },

		{ Pin::ShiftScl,  15 },
		{ Pin::ShiftSda,  4 },
		{ Pin::ShiftPl,   0 }
};

const PinDefMap<Pin> Pins3 = {
		{ Pin::BtnUp,     0 },
		{ Pin::BtnDown,   3 },
		{ Pin::BtnLeft,   1 },
		{ Pin::BtnRight,  2 },
		{ Pin::BtnA,      6 },
		{ Pin::BtnB,      5 },
		{ Pin::BtnC,      4 },

		{ Pin::SpkrOut,   25 },
		{ Pin::SpkrEn,    13 },
		{ Pin::Backlight, 18 },
		{ Pin::AdcBatt,   36 },

		{ Pin::DetChrg,   39 },
		{ Pin::DetSD,     34 },

		{ Pin::SD_Sck,    14 },
		{ Pin::SD_Miso,   5 },
		{ Pin::SD_Mosi,   32 },
		{ Pin::SD_Cs,     2 },

		{ Pin::TFT_Sck,   26 },
		{ Pin::TFT_Mosi,  33 },
		{ Pin::TFT_Dc,    21 },
		{ Pin::TFT_Rst,   27 },

		{ Pin::ShiftScl,  15 },
		{ Pin::ShiftSda,  4 },
		{ Pin::ShiftPl,   0 },

		//NOTE: Design error on HW v2.3, GPIO35 is input-only and cannot be used here
		{ Pin::RefEn,     35 }
};
