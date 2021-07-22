#ifndef AUTOCAR_CAMERAFEED_H
#define AUTOCAR_CAMERAFEED_H

#include <Arduino.h>
#include <esp_camera.h>

class Camera {
public:
	Camera();
	virtual ~Camera();

	/**
	 * Load a frame from the camera.
	 */
	bool loadFrame();

	/**
	 * Release last frame.
	 */
	void releaseFrame();

	/**
	 * Get a raw RGB565 frame buffer size 160 x 120
	 */
	uint16_t* getRGB565() const;

	/**
	 * Get a raw RGB888 frame buffer size 160 x 120
	 */
	uint8_t* getRGB888() const;

	/**
	 * Get the raw camera frame.
	 */
	const camera_fb_t* getFrame() const;

	bool isInited() const;

	operator bool() const;

private:
	camera_fb_t* frame = nullptr;
	uint16_t* frame565;
	uint8_t* frame888;

	static bool inited;
};


#endif //AUTOCAR_CAMERAFEED_H
