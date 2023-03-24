#ifndef BATMOBILE_LIBRARY_COLORCONVERTER_H
#define BATMOBILE_LIBRARY_COLORCONVERTER_H

#include <vector>
#include <cstdint>
#include <glm.h>

class ColorConverter {
public:
    static glm::vec3 toRGB(uint8_t hue);

private:
    constexpr static const float Luminance = 0.5f;
    constexpr static const float Saturation = 1.0f;
    constexpr static const float Tmp1 = Luminance + Saturation - Luminance * Saturation; //If Luminance >= 0.5f
//  constexpr static const float Tmp1 = Luminance * (1.0f + Saturation);                 //If Luminance < 0.5f
    constexpr static const float Tmp2 = 2 * Luminance - Tmp1;
};


#endif //BATMOBILE_LIBRARY_COLORCONVERTER_H
