#ifndef SYNTHIA_LIBRARY_PINS_HPP
#define SYNTHIA_LIBRARY_PINS_HPP

#include <cstdint>

enum class Pin : uint8_t {
	BtnUp, BtnDown, BtnLeft, BtnRight, BtnMid, BtnBack,
};

// pins which are the same across every revision (so far)

#define I2C_SDA 14
#define I2C_SCL 15

#define MOTOR_FL 0
#define MOTOR_BL 1
#define MOTOR_FR 2
#define MOTOR_BR 3

#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      4
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

#endif //SYNTHIA_LIBRARY_PINS_HPP
