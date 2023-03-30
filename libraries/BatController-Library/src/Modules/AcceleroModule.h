#ifndef BATCONTROLLER_FIRMWARE_ACCELEROMODULE_H
#define BATCONTROLLER_FIRMWARE_ACCELEROMODULE_H

#include "Module.h"
#include <glm.h>

class AcceleroModule : public Module {
public:
	AcceleroModule();
	[[nodiscard]] const glm::vec<3, int16_t>& getAccelerometer() const;
	[[nodiscard]] int8_t getTemperature() const;

protected:
	void init() override;
	void transmission() override;

private:
	glm::vec<3, int16_t> accel{};
	int8_t temp = 25;

};


#endif //BATCONTROLLER_FIRMWARE_ACCELEROMODULE_H
