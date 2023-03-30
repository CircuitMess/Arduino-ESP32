#ifndef BATMOBILE_COMMON_DISCONNECTLISTENER_H
#define BATMOBILE_COMMON_DISCONNECTLISTENER_H

// TODO: rename this structure to ConChangedListener or something

class DisconnectListener {
public:
    virtual void onDisconnected();
	virtual void onConnected();
};


#endif //BATMOBILE_COMMON_DISCONNECTLISTENER_H
