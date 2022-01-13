#ifndef BYTEBOI_LIBRARY_BYTEBOILED_H
#define BYTEBOI_LIBRARY_BYTEBOILED_H

#include <Arduino.h>

enum LEDColor {
		OFF, RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN, WHITE
};
class ByteBoiLED {
public:
	void begin();
	void setRGB(LEDColor colour);
	LEDColor getRGB();
private:
	LEDColor currentColor;

};

extern ByteBoiLED LED;

#endif //BYTEBOI_LIBRARY_BYTEBOILED_H
