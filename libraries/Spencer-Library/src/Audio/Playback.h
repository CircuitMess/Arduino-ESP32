#ifndef SPENCER_PLAYBACK_H
#define SPENCER_PLAYBACK_H

#include "I2S.h"
#include "AudioFileSourceSerialFlash.h"
#include "CompositeAudioFileSource.h"
#include <AudioFileSource.h>
#include <AudioOutputI2S.h>
#include <AudioGeneratorWAV.h>
#include <AudioGeneratorMP3.h>
#include "SampleStore.h"
#include <Loop/LoopListener.h>

class PlaybackImpl : public LoopListener
{
public:
	PlaybackImpl();
	~PlaybackImpl();
	void begin(I2S* i2s);
	/**
	 * Starts WAV audio playback from an AudioFileSource pointer.
	 * @param file AudioFileSource pointer to the desired file.
	 */
	void playWAV(AudioFileSource* file);
	/**
	 * Starts WAV audio playback of a file from the serial flash chip.
	 * @param path Path of the file on the flash chip.
	 */
	void playWAV(const char* path);
	/**
	 * Starts MP3 audio playback from an AudioFileSource pointer.
	 * @param file AudioFileSource pointer to the desired file.
	 */
	void playMP3(AudioFileSource* file);
	/**
	 * Starts MP3 audio playback of a file from the serial flash chip.
	 * @param path Path of the file on the flash chip.
	 */
	void playMP3(const char* path);
	/**
	 * Stops audio playback.
	 * @param executeCallback Flag to execute callback after playing. False by default.
	 */
	void stopPlayback(bool executeCallback = false);
	/**
	 * Sets playback to be executed when a track is finished. Only works when a track is currently being played!
	 * @param callback Function to be executed after a track is finished.
	 */
	void setPlaybackDoneCallback(void (*callback)());
	/**
	 * Sets playback volume.
	 * @param vol Volume, float from 0.0 to 4.0;
	 */
	void setVolume(float vol);
	/**
	 * Gets playback volume.
	 * @return Volume, float from 0.0 to 4.0;
	 */
	float getVolume();
	void loop(uint _time) override;
	bool isRunning();

private:
	I2S* i2s;
	AudioGeneratorWAV *wav = nullptr;
	AudioGeneratorMP3 *mp3 = nullptr;
	AudioOutputI2S *out;
	AudioFileSource *file;
	void(*playbackDoneCallback)();
	float volume = 0.4;

};

extern PlaybackImpl Playback;

#endif
