
#include "Aruco.h"
#include "ArucoDictionary.h"
#include <functional>

void Aruco::generate(uint16_t id, Color* buffer, uint8_t scale){

    uint16_t w = 7*scale;
    uint16_t h = 7*scale;
    auto drawPix = [&buffer, &w, &h](uint16_t x, uint16_t y, Color c){
        buffer[x+y*w] = c;
    };

    auto fillRect = [&buffer, &w, &h, &drawPix](uint16_t _x, uint16_t _y, uint16_t rectW, uint16_t rectH, Color c){
        for(int y = 0; y < rectH; y++){
            for(int x = 0; x < rectW; x++){
                drawPix(x + _x, y + _y, c);
            }
        }
    };

    const uint8_t* bytes = DICTIONARY[id];
    uint8_t bitsCount = side * side;
    uint8_t bits[bitsCount];

    // Parse marker's bytes
    int iterator = 0;
    for(int i = 0; i < 4; i++){
        int start = bitsCount - iterator;
        for(int j = std::min(7,start-1); j >= 0; j--){
            bits[iterator] = ((bytes[i] >> j) & 1);
            iterator++;
        }
    }
    memset(buffer, TFT_BLACK, (2+side) * (2+side) * sizeof(Color) * scale);

    //	Tiles
    for(int i = 0; i < side; i++){
        for(int j = 0; j < side; j++){
            uint8_t white = bits[i*side + j];
            if(white == 0)	continue;

            uint16_t index = scale*scale*(i+1)*(2+side) + scale*(j+1);
            for(int y = 0; y < scale; y++){
                for(int x = 0; x < scale; x++){
                    buffer[index + y*(2+side)*scale + x] = TFT_WHITE;
                }
            }
        }
    }

}
