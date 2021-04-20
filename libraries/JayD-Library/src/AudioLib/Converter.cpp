#include "Converter.h"
#include "../AudioSetup.hpp"

Converter::Converter(Source* source) : source(source){

}

Converter::~Converter(){
}

size_t Converter::generate(int16_t* outBuffer){
}
