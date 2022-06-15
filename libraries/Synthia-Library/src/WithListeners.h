#ifndef CHATTER_FIRMWARE_WITHLISTENERS_H
#define CHATTER_FIRMWARE_WITHLISTENERS_H

#include <unordered_set>
#include <functional>

template<typename T>
class WithListeners {
public:
	void addListener(T* listener) {
		listeners.insert(listener);
		deletedListeners.erase(listener);
	}

	void removeListener(T* listener){
		auto it = listeners.find(listener);
		if(it == listeners.end()) return;
		if(deletedListeners.find(listener) == deletedListeners.end()){
			deletedListeners.insert(listener);
		}
	}

protected:
	/**
	 * Notify all listeners that something has changed.
	 *
	 * @param func Function to be called on all listeners
	 */
	void iterateListeners(std::function<void(T*)> func){
		for(auto listener : listeners){
			if(deletedListeners.find(listener) != deletedListeners.end()){
				continue;
			}
			func(listener);
		}

		for(auto deleted : deletedListeners){
			listeners.erase(listeners.find(deleted));
		}
		deletedListeners.clear();
	}

private:
	std::unordered_set<T*> listeners;
	std::unordered_set<T*> deletedListeners;
};

#endif //CHATTER_FIRMWARE_WITHLISTENERS_H
