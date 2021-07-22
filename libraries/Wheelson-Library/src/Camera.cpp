#include "Camera.h"

#include <esp_camera.h>
#include <Display/Color.h>
#include "Wheelson.h"
bool Camera::inited = false;

Camera::Camera() : frame565((uint16_t*) ps_malloc(160 * 120 * sizeof(uint16_t))), frame888(static_cast<uint8_t*>(ps_malloc(160 * 120 * 3))){

	camera_config_t config;
	config.ledc_channel = LEDC_CHANNEL_0;
	config.ledc_timer = LEDC_TIMER_0;
	config.pin_d0 = Y2_GPIO_NUM;
	config.pin_d1 = Y3_GPIO_NUM;
	config.pin_d2 = Y4_GPIO_NUM;
	config.pin_d3 = Y5_GPIO_NUM;
	config.pin_d4 = Y6_GPIO_NUM;
	config.pin_d5 = Y7_GPIO_NUM;
	config.pin_d6 = Y8_GPIO_NUM;
	config.pin_d7 = Y9_GPIO_NUM;
	config.pin_xclk = XCLK_GPIO_NUM;
	config.pin_pclk = PCLK_GPIO_NUM;
	config.pin_vsync = VSYNC_GPIO_NUM;
	config.pin_href = HREF_GPIO_NUM;
	config.pin_sscb_sda = SIOD_GPIO_NUM;
	config.pin_sscb_scl = SIOC_GPIO_NUM;
	config.pin_pwdn = PWDN_GPIO_NUM;
	config.pin_reset = RESET_GPIO_NUM;
	config.xclk_freq_hz = 18000000;

	config.pixel_format = PIXFORMAT_RGB888;
	config.frame_size = FRAMESIZE_QQVGA;
	config.fb_count = 1;
	if(!inited){
		// Init Camera
		esp_err_t err = esp_camera_init(&config);
		inited = err == ESP_OK;

		if(err != ESP_OK){
			Serial.printf("Camera init failed with error 0x%x", err);
			return;
		}
	}

	Serial.println("Camera init OK!");
	sensor_t* sensor = esp_camera_sensor_get();
	sensor->set_hmirror(sensor, 1);
	sensor->set_vflip(sensor, 1);
	sensor->set_quality(sensor, 10);
	sensor->set_special_effect(sensor, 0);
	sensor->set_saturation(sensor, 2);
}

Camera::~Camera(){
	free(frame565);
	free(frame888);

	if(frame){
		esp_camera_fb_return(frame);
	}
}

bool Camera::loadFrame(){
	frame = esp_camera_fb_get();
	if(!frame){
		Serial.println("Camera capture failed");
		return false;
	}

	memcpy(frame888, frame->buf, min(frame->len, (size_t) 160 * 120 * 3));

	for(int i = 0; i < min(frame->len, (size_t) 160 * 120 * 3); i += 3){
		frame565[i / 3] = C_RGB(frame888[i+2], frame888[i + 1], frame888[i]);
	}

	return true;
}

void Camera::releaseFrame(){
	if(!frame) return;

	esp_camera_fb_return(frame);
	frame = nullptr;
}

uint16_t* Camera::getRGB565() const{
	return frame565;
}

uint8_t* Camera::getRGB888() const{
	return frame888;
}

const camera_fb_t* Camera::getFrame() const{
	return frame;
}

bool Camera::isInited() const{
	return inited;
}

Camera::operator bool() const{
	return isInited();
}
