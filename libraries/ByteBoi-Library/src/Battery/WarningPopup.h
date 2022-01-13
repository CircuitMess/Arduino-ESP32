#ifndef BYTEBOI_LIBRARY_WARNINGPOPUP_H
#define BYTEBOI_LIBRARY_WARNINGPOPUP_H

#include <Support/Modal.h>
#include <Loop/LoopListener.h>

class WarningPopup : public Modal, public LoopListener {
public:
	WarningPopup(Context &context);
	~WarningPopup();
	void draw() override;

	void start() override;

	void stop() override;

	void loop(uint) override;

	void returned(void* data) override;
private:
	uint warningTimer = 0;
	static const uint8_t warningTime;
	Modal* prevModal = nullptr;
	static WarningPopup* instance;
	Color* buffer = nullptr;
};


#endif //BYTEBOI_LIBRARY_WARNINGPOPUP_H
