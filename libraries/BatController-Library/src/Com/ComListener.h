#ifndef BATCONTROLLER_LIBRARY_COMLISTENER_H
#define BATCONTROLLER_LIBRARY_COMLISTENER_H

#include <Error.h>

class ComListener {
public:
	virtual void onBattery(uint8_t percent, bool charging){};

	virtual void onSignalStrength(uint8_t percent){};

	virtual void onError(BatError error){};
};


#endif //BATCONTROLLER_LIBRARY_COMLISTENER_H
