#ifndef WHEELSON_FIRMWARE_TAGDETECTION_H
#define WHEELSON_FIRMWARE_TAGDETECTION_H

#include <Display/Display.h>
#include <vector>
#include <opencv2.hpp>
#include "MarkerDetection/Aruco/Marker.h"

class Markers {
public:
	enum PixFormat {
		GRAY, // 1 unsigned byte per pixel
		RGB565, // 2 bytes per pixel, RGB565 format
		RGB888 // 3 bytes per pixel, RGB565 format
	};

	static std::vector<Aruco::Marker> detect(uint8_t* frame, uint16_t width, uint16_t height, PixFormat format = RGB565, Color* output = nullptr);
};


#endif //WHEELSON_FIRMWARE_TAGDETECTION_H
