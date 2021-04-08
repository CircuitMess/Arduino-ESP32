#include <Arduino.h>
#include <CircuitOS.h>
#include "src/Spencer.h"
#include "src/PreparedStatement.h"

const char* listenResult;
IntentResult* intentResult;
bool processingIntent = false;
bool detectingIntent = false;
PreparedStatement* statement = nullptr;
bool synthesizing = false;
String speechText;

void speechPlay(TTSError error, CompositeAudioFileSource* source){
	synthesizing = false;
	Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
	if(error != TTSError::OK){
		Serial.printf("Text to speech error %d: %s\n", error, TTSStrings[(int) error]);
		delete source;
		delete statement;
		statement = nullptr;
		return;
	}
	LEDmatrix.startAnimation(new Animation(new SerialFlashFileAdapter("GIF-talk.gif")), true);
	Playback.playMP3(source);
	Playback.setPlaybackDoneCallback([](){
		LEDmatrix.startAnimation(new Animation(new SerialFlashFileAdapter("GIF-angry.gif")), true);

	});
	delete statement;
	statement = nullptr;

}

void listenProcess(){
	Serial.println(String(intentResult->transcript));
	if(synthesizing){
		Serial.println("Another speech synthesis operation is already pending");
	}else{
		synthesizing = true;
		delete statement;
		statement = new PreparedStatement();
		speechText = (String(intentResult->transcript) + String("My name is Spencer"));
		statement->addTTS(speechText);
		statement->prepare(speechPlay);
	}

}

void listenError(){

}

void listenCheck(){
	if(listenResult != nullptr && !processingIntent){
		processingIntent = true;
		delete intentResult;
		intentResult = nullptr;
		SpeechToIntent.addJob({ listenResult, &intentResult });

		LEDmatrix.startAnimation(new Animation(new SerialFlashFileAdapter("GIF-heart.gif")), true);

	}
	if(processingIntent && intentResult != nullptr){
		detectingIntent = false;
		processingIntent = false;
		listenResult = nullptr;
		if(intentResult->error != IntentResult::Error::OK && intentResult->error != IntentResult::Error::INTENT){
			Serial.printf("Speech to text error %d: %s\n", intentResult->error, STIStrings[(int) intentResult->error]);
			listenError();
			delete intentResult;
			intentResult = nullptr;
			return;
		}
		if(intentResult->intent == nullptr){
			intentResult->intent = (char*) malloc(5);
			memcpy(intentResult->intent, "NONE", 5);
		}
		if(intentResult->transcript == nullptr){
			intentResult->transcript = (char*) malloc(1);
			memset(intentResult->transcript, 0, 1);
		}
		listenProcess();
		delete intentResult;
		intentResult = nullptr;
	}
}

void BTN_press(){
	if (Net.getState() == WL_CONNECTED) {
		LEDmatrix.startAnimation(new Animation(new SerialFlashFileAdapter("GIF-loading0.gif")), true);
		if(detectingIntent){
			Serial.println("Another listen and intent detection operation is already pending");
		}else{
			detectingIntent = true;
			listenResult = nullptr;
			Recording.addJob({ &listenResult });
		}
	}

}


void setup() {
	Serial.begin(115200);
	Serial.println();
	Spencer.begin();
	Spencer.loadSettings();
	Input::getInstance()->setBtnPressCallback(BTN_PIN, BTN_press);

	LEDmatrix.startAnimation(new Animation(new SerialFlashFileAdapter("GIF-wifi.gif")), true);
	Net.connect([](wl_status_t state){
		if (Net.getState() == WL_CONNECTED) {
			LEDmatrix.startAnimation(new Animation(new SerialFlashFileAdapter("GIF-smile.gif")), true);
		} else {
			LEDmatrix.startAnimation(new Animation(new SerialFlashFileAdapter("GIF-noWifi.gif")), true);
		}

		Serial.printf("%s\n", Net.getState() == WL_CONNECTED ? "con" : "no con");
	});


}

void loop() {
	LoopManager::loop();
	listenCheck();


}