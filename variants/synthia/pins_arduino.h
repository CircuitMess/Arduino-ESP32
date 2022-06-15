#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

#define analogInputToDigitalPin(p)  (((p)<20)?(esp32_adc2gpio[(p)]):-1)
#define digitalPinToInterrupt(p)    (((p)<40)?(p):-1)
#define digitalPinHasPWM(p)         (p < 34)

static const uint8_t SDA = 23;
static const uint8_t SCL = 22;

static const uint8_t SCK = -1;
static const uint8_t MISO = -1;
static const uint8_t MOSI = -1;
static const uint8_t SS = -1;

#endif /* Pins_Arduino_h */
