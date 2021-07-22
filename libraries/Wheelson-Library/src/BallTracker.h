#ifndef WHEELSON_LIBRARY_BALLTRACKER_H
#define WHEELSON_LIBRARY_BALLTRACKER_H

#include <Arduino.h>
#include <opencv2.hpp>
#include <opencv2/core/types.hpp>
#include <Display/Color.h>

struct Ball {
	/**
	 * Average hue of the detected ball. Range 0-180
	 */
	uint8_t hue;

	/**
	 * Fitness of the detected ball. I.e. how sure the system is that the detected object is a ball.
	 * Values range from 0 upwards with 0 being a perfect match.
	 */
	float fitness;

	cv::Point2i center;

	uint radius;
};

class BallTracker {
public:
	enum PixFormat {
		RGB565, // 2 bytes per pixel, RGB565 format
		RGB888 // 3 bytes per pixel, RGB565 format
	};

	static std::vector<Ball> detect(uint8_t* frame, uint16_t width, uint16_t height, uint8_t hue, PixFormat format = RGB565, Color* output = nullptr);
};


#endif //WHEELSON_LIBRARY_BALLTRACKER_H
