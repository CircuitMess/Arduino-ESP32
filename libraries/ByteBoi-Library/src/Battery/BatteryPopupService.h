#ifndef BYTEBOI_LIBRARY_BATTERYPOPUPSERVICE_H
#define BYTEBOI_LIBRARY_BATTERYPOPUPSERVICE_H

#include <Loop/LoopListener.h>

class WarningPopup;
class ShutdownPopup;

class BatteryPopupService : public LoopListener{
public:
	void loop(uint time) override;

	/**
	 * Enable pop-ups. When enabling, this will disable BatteryService auto-shutdown, but won't re-enable
	 * it when disabling the pop-ups.
	 *
	 * Game should be contained within Contexts for this to work.
	 */
	void enablePopups(bool enable);

private:
	static const uint16_t checkInterval;
	uint checkMicros = 0;

	static ShutdownPopup *shutdownPopup;
	static WarningPopup *warningPopup;
	bool warningShown = false;
	bool enabled = false;
};

#endif //BYTEBOI_LIBRARY_BATTERYPOPUPSERVICE_H
