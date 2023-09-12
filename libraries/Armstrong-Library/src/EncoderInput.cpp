#include "EncoderInput.h"
#include <Loop/LoopManager.h>

EncoderInput::EncoderInput() : scanTask("EncoderTask", [](Task* task){ static_cast<EncoderInput*>(task->arg)->scanTaskFunc(); }, 4096, this){
	for(const auto& pair : PinMap){
		prevState.insert({ pair.first, INT32_MAX });
	}
}

void EncoderInput::begin(){
	for(const auto& pair : PinMap){
		const auto motor = pair.first;
		const auto pins = pair.second;

		pinMode(pins.first, INPUT);
		pinMode(pins.second, INPUT);

		prevState[motor] = INT32_MAX;
	}

	scanTask.start(1, 0);
	LoopManager::addListener(this);
}

void EncoderInput::end(){
	scanTask.stop(true);
	LoopManager::removeListener(this);
}

void EncoderInput::loop(uint micros){
	sendTimer += micros;
	if(sendTimer < SendInterval) return;
	sendTimer = 0;

	queueMut.lock();
	std::deque<Action> queue(actionQueue);
	actionQueue.clear();
	queueMut.unlock();

	while(!queue.empty()){
		auto action = queue.front();
		queue.pop_front();
		if(action.amount == 0) continue;

		iterateListeners([action](EncoderListener* listener){
			listener->encoderMove(action.enc, action.amount);
		});
	}
}

void EncoderInput::scan(Motor enc){
	const auto pins = PinMap.find(enc)->second;

	int8_t movement = 0;

	const int state = digitalRead(pins.first);
	if(state != prevState[enc] && prevState[enc] != INT32_MAX){
		if(digitalRead(pins.second) != state){
			movement = 1;
		}else{
			movement = -1;
		}
	}

	prevState[enc] = state;

	if(movement != 0){
		std::lock_guard<std::mutex> m(queueMut);

		if(!actionQueue.empty() && actionQueue.back().enc == enc){
			auto& last = actionQueue.back();

			int total = last.amount + movement;
			if(total == 0){
				actionQueue.pop_back();
			}else if(total < INT8_MAX && total > INT8_MIN){
				last.amount = total;
			}else{
				actionQueue.push_back({ enc, movement });
			}

			return;
		}

		actionQueue.push_back({ enc, movement });
	}
}

void EncoderInput::scanTaskFunc(){
	while(scanTask.running){
		for(int i = 0; i < 4; i++){
			scan((Motor) i);
		}

		delay(1);
	}
}
