#ifndef BATCONTROLLER_LIBRARY_ARUCO_H
#define BATCONTROLLER_LIBRARY_ARUCO_H

#include <Display/Sprite.h>

class Aruco {
public:
    static void generate(uint16_t id, Color* buffer, uint8_t scale = 1);

private:
    static const int side = 5;

};

#endif //BATCONTROLLER_LIBRARY_ARUCO_H
