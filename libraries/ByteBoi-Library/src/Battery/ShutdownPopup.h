#ifndef BYTEBOI_LIBRARY_SHUTDOWNPOPUP_H
#define BYTEBOI_LIBRARY_SHUTDOWNPOPUP_H

#include <Support/Modal.h>
#include <Loop/LoopListener.h>

class ShutdownPopup : public Modal, public LoopListener {
public:
	ShutdownPopup(Context &context);
	~ShutdownPopup();
	void draw() override;

	void start() override;

	void stop() override;

	void loop(uint) override;
private:
	Color* buffer = nullptr;
	uint shutdownTimer = 0;
	static const uint8_t shutdownTime;

};


#endif //BYTEBOI_LIBRARY_SHUTDOWNPOPUP_H
