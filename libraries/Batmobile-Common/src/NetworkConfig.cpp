#include "NetworkConfig.h"

const IPAddress batmobileIP(10, 0, 0, 5);
const IPAddress defaultControllerIP(10, 0, 0, 4);
IPAddress controllerIP = defaultControllerIP;
const IPAddress gateway(10, 0, 0, 1);
const IPAddress subnet(255, 255, 255, 0);
const uint16_t controlPort = 63;
const uint16_t feedPort = 64;

const uint8_t FrameHeader[FEED_ENV_LEN] = { 0x18, 0x20, 0x55, 0xf2, 0x5a, 0xc0, 0x4d, 0xaa };
const uint8_t FrameTrailer[FEED_ENV_LEN] = { 0x42, 0x2c, 0xd9, 0xe3, 0xff, 0xa0, 0x11, 0x01 };
const uint8_t FrameSizeShift[4] = { 2, 3, 1, 0 };
