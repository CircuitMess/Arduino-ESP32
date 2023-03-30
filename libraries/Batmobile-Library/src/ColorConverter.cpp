#include "ColorConverter.h"


//function was made with the help from this site
//https://www.niwa.nu/2013/05/math-behind-colorspace-conversions-rgb-hsl/

glm::vec3 ColorConverter::toRGB(uint8_t hue) {
    float tmpHue = (float)hue/180;
    float tmpR = tmpHue + 0.33;
    if(tmpR > 1) tmpR -= 1;
    float tmpG = tmpHue;
    float tmpB = tmpHue - 0.33;
    if(tmpB < 0) tmpB += 1;

    auto test = [](float tmpColor){
        if(6 * tmpColor < 1.0f){
            tmpColor = Tmp2 + (Tmp1 - Tmp2) * 6 * tmpColor;
        }else if(2 * tmpColor < 1.0f){
            tmpColor = Tmp1;
        }else if(3 * tmpColor < 2.0f){
            tmpColor = Tmp2 + (Tmp1 - Tmp2) * 6 * (0.666f - tmpColor);
        }else{
            tmpColor = Tmp2;
        }
        return  tmpColor;
    };

    tmpR = test(tmpR);
    tmpG = test(tmpG);
    tmpB = test(tmpB);

    glm::vec3 vectorRGB = {(uint8_t)(tmpR * 255), (uint8_t)(tmpG * 255), (uint8_t)(tmpB * 255)};

    return vectorRGB;
}
