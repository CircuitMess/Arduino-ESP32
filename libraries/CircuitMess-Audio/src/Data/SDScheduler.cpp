#include <SPI.h>
#include "SDScheduler.h"

SDScheduler Sched;

void SDScheduler::beforeJob(){
	SPI.setFrequency(60000000);
}

void SDScheduler::afterJob(){

}
