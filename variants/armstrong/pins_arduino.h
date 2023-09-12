#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

#define analogInputToDigitalPin(p)  (((p)<20)?(esp32_adc2gpio[(p)]):-1)
#define digitalPinToInterrupt(p)    (((p)<40)?(p):-1)
#define digitalPinHasPWM(p)         (p < 34)

static const uint8_t SDA = 26;
static const uint8_t SCL = 27;

static const uint8_t SCK = -1;
static const uint8_t MISO = -1;
static const uint8_t MOSI = -1;
static const uint8_t SS = 15;

#endif /* Pins_Arduino_h */
