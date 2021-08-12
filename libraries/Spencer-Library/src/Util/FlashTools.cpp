#include <SerialFlash.h>
#include "FlashTools.h"
#include <SD.h>

void FlashTools::erase(){
	SerialFlash.eraseAll();
	while(!SerialFlash.ready());
}

void FlashTools::upload(){
	Serial.setTimeout(30000);
	Serial.println("Waiting for serial upload");

	while(!Serial.available());
	char upload[6];
	Serial.readBytes(upload, 6);
	if(strncmp(upload, "upload", 6)){
		Serial.printf("Expected 'upload', gotten '%.6s'\n", upload);
		return;
	}

	Serial.print("erase");
	erase();
	Serial.print("ready");

	while(!Serial.available());

	char filename[MAX_FILENAME + 1];
	uint32_t filesize;
	while(true){
		Serial.readBytes(filename, MAX_FILENAME + 1);
		if(strncmp(filename, "end", 3) == 0) break;

		Serial.readBytes(reinterpret_cast<char*>(&filesize), sizeof(uint32_t));

		if(SerialFlash.exists(filename)){
			SerialFlash.remove(filename);
		}

		SerialFlash.create(filename, filesize);
		SerialFlashFile file = SerialFlash.open(filename);

		size_t fileWritten = 0;
		uint32_t sum = 0;
		while(fileWritten < filesize){
			byte data[256];
			size_t bytes = Serial.readBytes(data, min((size_t) 256, filesize - fileWritten));

			file.write(data, bytes);
			fileWritten += bytes;

			for(int i = 0; i < bytes; i += sizeof(uint32_t)){
				sum += *((uint32_t*)(&data[i]));
			}
		}
		file.close();

		Serial.write(reinterpret_cast<uint8_t*>(&sum), sizeof(uint32_t));
	}

	Serial.setTimeout(1000);
}

void FlashTools::listFiles(){
	SerialFlash.opendir();
	while(1){
		char filename[64];
		uint32_t filesize;

		if(SerialFlash.readdir(filename, sizeof(filename), filesize)){
			Serial.print("  ");
			Serial.print(filename);
			for(int i = 0; i < 40 - (int) strlen(filename); i++) Serial.print(" ");
			Serial.print("  ");
			Serial.print(filesize);
			Serial.print(" bytes");
			Serial.println();
		}else{
			break; // no more files
		}
	}
}

void FlashTools::uploadSD(){
	SPIClass spiSD(2);
	spiSD.begin(13, 14, 15, 12);

	Serial.println("Waiting for SD");
	while(!SD.begin(14, spiSD, 24000000)){
		delay(500);
	}

	uint32_t time = millis();

	Serial.println("Erasing...");
	erase();
	Serial.println("Uploading...");

	File root = SD.open("/");
	copyDirSD(root);
	root.close();
	spiSD.end();

	Serial.println("Upload done");
	Serial.printf("Total time: %.2f m\n", (float) (millis() - time) / (1000.0f * 60.0f));
}

uint32_t FlashTools::uploadSDSilent(){
	SPIClass spiSD(2);
	spiSD.begin(13, 14, 15, 12);

	int tries = 0;
	while(!SD.begin(14, spiSD, 12000000)){
		if(tries++ >= 5) return 0;
		delay(500);
	}

	uint32_t time = millis();

	File root = SD.open("/");
	copyDirSD(root);
	root.close();
	spiSD.end();

	return millis() - time;
}

void FlashTools::copyDirSD(File& dir, const char* prefix){
	File fileSD;
	char* buf = static_cast<char*>(malloc(2048));

	while((fileSD = dir.openNextFile())){
		const char* fileSDname;
		const char* nameptr = fileSD.name()-1;
		while(*++nameptr != '\0'){
			if(*nameptr == '/') fileSDname = nameptr;
		}
		fileSDname++;

		char filename[MAX_FILENAME];

		if(fileSD.isDirectory()){
			if(prefix[0] == '\0'){
				sprintf(filename, "%s-", fileSDname);
			}else{
				sprintf(filename, "%s%s-", prefix, fileSDname);
			}

			copyDirSD(fileSD, filename);
			fileSD.close();
			continue;
		}

		sprintf(filename, "%s%s", prefix, fileSDname);

		if(SerialFlash.exists(filename)){
			SerialFlash.remove(filename);
		}

		SerialFlash.create(filename, fileSD.size());
		SerialFlashFile fileFlash = SerialFlash.open(filename);

		uint32_t read;

		while((read = fileSD.readBytes(buf, 2048))){
			fileFlash.write(buf, read);
		}

		fileFlash.close();
		fileSD.close();
	}

	free(buf);
}

void FlashTools::uploadToFlash(){
	SPIClass spi2(2);
	spi2.begin(13, 14, 15, 12);
	SerialFlashChip flash2(spi2);
	flash2.setSettings(SPISettings(20000000, MSBFIRST, SPI_MODE0));
	if(!flash2.begin(12)){
		Serial.println("Flash2 fail");
		return;
	}

	Serial.println("Erasing flash2");
	flash2.eraseAll();
	while(!flash2.ready());

	char filename[128];
	uint32_t filesize;
	unsigned char buffer[256];

	Serial.println("uploading");
	uint32_t startTime = millis();

	currentFlash = &SerialFlash;
	SerialFlash.opendir();
	while(SerialFlash.readdir(filename, 128, filesize)){
		currentFlash = &flash2;
		if(!flash2.create(filename, filesize)){
			Serial.println("Failed creating");
			continue;
		}

		currentFlash = &SerialFlash;
		SerialFlashFile source = SerialFlash.open(filename);

		currentFlash = &flash2;
		SerialFlashFile destination = flash2.open(filename);

		if(!source){
			Serial.println("Failed opening source");
			continue;
		}

		if(!destination){
			Serial.println("Failed opening destination");
			continue;
		}

		uint32_t bytesRead;
		currentFlash = &SerialFlash;
		while((bytesRead = source.read(buffer, 256))){
			currentFlash = &flash2;
			destination.write(buffer, bytesRead);
			currentFlash = &SerialFlash;
		}

		source.close();
		destination.close();

		currentFlash = &SerialFlash;
	}

	Serial.printf("Done in %lu\n", millis() - startTime);
}