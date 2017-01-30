/*
 * audioRecording.c
 *
 *  Created on: 3 wrz 2016
 *      Author: Patryk Kotlarz
 */

#include "audioRecording.h"

static uint16_t inputDeviceStat;
static uint8_t volumeStat;
static uint32_t audioFreqStat;
static uint16_t* audioBufferStat;
static uint32_t audioBufferSizeStat;

/**
 * @brief Audio recording initialization
 * @param inpuTdevice: AUDIO_RECORDER_INPUT_MICROPHONE or AUDIO_RECORDER_INPUT_LINE
 * @param volume: 0 - mute, 80 - 0 dB, 100 - 17.625 dB
 * @retval AUDIO_OK - no errors
 */
uint8_t audioRecorderInit(uint16_t inputDevice, uint8_t volume,
		uint32_t audioFreq) {
	inputDeviceStat = inputDevice;
	volumeStat = volume;
	audioFreqStat = audioFreq;
	return BSP_AUDIO_IN_Init(inputDevice, volume, audioFreq);
}

/**
 * @brief The function sets the 16 bit audio buffer (must have allocated memory) by \p audioBuffer and \p audioBufferSize.
 * @param audioBuffer: pointer to global audio buffer
 * @param audioBufferSize: size of audio buffer
 * @retval AUDIO_OK - no errors
 */
uint8_t audioRecorderStartRecording(uint16_t* audioBuffer, uint32_t audioBufferSize) {
	audioBufferStat = audioBuffer;
	audioBufferSizeStat = audioBufferSize;

	return BSP_AUDIO_IN_Record(audioBuffer, audioBufferSize);
}

/**
 * @brief Controls the audio recorder volume.
 * @param volume: volume of audio sampling (0 to 100 where 80 equals 0 dB)
 * @retval AUDIO_OK - no errors
 */
uint8_t audioRecorderSetVolume(uint8_t volume) {
	volumeStat = volume;
	return BSP_AUDIO_IN_SetVolume(volume);
}

/**
 * @brief Changes audio sampling frequency
 * @param frequency: new audio sampling frequency
 * @retval AUDIO_OK - no errors
 */
uint8_t audioRecorderSetSamplingFrequency(uint32_t frequency) {
	uint8_t status;
	
	BSP_AUDIO_IN_Pause();
	BSP_AUDIO_IN_Stop(CODEC_PDWN_HW);
	audioFreqStat = frequency;

	status = audioRecorderInit(inputDeviceStat, volumeStat,
			audioFreqStat);
	if (status != AUDIO_OK)
		return status;

	return audioRecorderStartRecording(audioBufferStat, audioBufferSizeStat);
}

/**
 * @brief Fills the \p soundStructure.
 * @param soundStructure pointer to SoundStr
 * @param audioBuffer 16 bit data array
 * @param audioBufferSize buffer size
 * @param frequency sampling frequency
 */
void audioRecordingSoundMailFill(SoundMailStr* soundStructure, uint16_t* audioBuffer, uint32_t audioBufferSize, uint32_t frequency) {
	uint32_t iterator;
	soundStructure->frequency = frequency;
	soundStructure->soundBufferSize = audioBufferSize;

	for (iterator = 0; iterator < audioBufferSize; iterator++) {
		soundStructure->soundBuffer[iterator] = audioBuffer[iterator];
	}
}

/**
 * @brief This function updates the sound buffer using "small" sound package of sound mail.
 * @param soundBuffer: pointer to SoundBuffer (destination)
 * @param SoundMail: pointer to SoundMail (source)
 */
void audioRecordingUpdateSoundBuffer(SoundBufferStr* soundBuffer, SoundMailStr* soundMail) {
	uint32_t i;
	soundBuffer->frequency = soundMail->frequency;

	for (i = 0; i < soundMail->soundBufferSize; i++) {
		soundBuffer->iterator++;
		if (soundBuffer->iterator >= soundBuffer->size)
			soundBuffer->iterator = 0;
		soundBuffer->soundBuffer[soundBuffer->iterator] =
				soundMail->soundBuffer[i];
	}
}

