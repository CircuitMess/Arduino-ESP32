#include "WheelsonInput.h"
#include "Nuvoton.h"
#include "../Wheelson.h"

WheelsonInput::WheelsonInput() : Input(NUM_BUTTONS), i2c(Nuvo.getI2C()){

}

uint8_t WheelsonInput::getNumEvents(){
	uint8_t msg[] = { GET_NUM_EVENTS_BYTE };
	uint8_t numEvents;
	i2c.request(msg, 1, &numEvents, 1);

	return numEvents;
}

void WheelsonInput::scanButtons(){
	uint8_t numberOfEvents = getNumEvents();
	if(numberOfEvents == 0) return;
	handleEvents(numberOfEvents);
}

void WheelsonInput::handleEvents(uint8_t numEvents){
	if(numEvents == 0) return;

	uint8_t msg[] = { GETEVENTS_BYTE, numEvents };
	uint8_t* data = static_cast<uint8_t*>(malloc(numEvents));
	i2c.request(msg, 2, data, numEvents);

	for(int i = 0; i < numEvents; i++){
		uint8_t event = data[i];

		uint8_t id = event & 0x7F;
		bool state = event >> 7;

		handleSingleEvent({ id, state });
	}

	free(data);
}

void WheelsonInput::handleSingleEvent(const WheelsonInput::InputEvent& event){
	if(event.state){
		btnPress(event.id);
	}else{
		btnRelease(event.id);
	}
}




