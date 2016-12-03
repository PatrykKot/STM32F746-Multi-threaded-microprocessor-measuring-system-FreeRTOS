/*
 * audioRecording.c
 *
 *  Created on: 3 wrz 2016
 *      Author: Patryk Kotlarz
 */

#include "audioRecording.h"

/**
 * @brief Audio recording initialization
 * @param inpuTdevice: AUDIO_RECORDER_INPUT_MICROPHONE or AUDIO_RECORDER_INPUT_LINE
 * @param volume: 0 - mute, 80 - 0 dB, 100 - 17.625 dB
 * @retval AUDIO_OK - no errors
 */
uint8_t audioRecorderInit(uint16_t inputDevice, uint8_t volume,
		uint32_t audioFreq) {
	return BSP_AUDIO_IN_Init(inputDevice, volume, audioFreq);
}

/**
 * @brief The function sets the 16 bit audio buffer (must have allocated memory) by \p audioBuffer and \p audioBufferSize.
 * @retval AUDIO_OK - no errors
 */
uint8_t audioRecorderStartRecording(uint16_t* audioBuffer,
		uint32_t audioBufferSize) {
	return BSP_AUDIO_IN_Record(audioBuffer, audioBufferSize);
}

/**
 * @brief Controls the audio recorder volume.
 * @retval AUDIO_OK - no errors
 */
uint8_t audioRecorderSetVolume(uint8_t volume) {
	return BSP_AUDIO_IN_SetVolume(volume);
}

/**
 * @brief Fills the \p soundStructure.
 * @param soundStructure pointer to SoundStr
 * @param audioBuffer 16 bit data array
 * @param audioBufferSize buffer size
 * @param frequency sampling frequency
 */
void audioRecordingSoundMailFill(SoundMailStr* soundStructure,
		uint16_t* audioBuffer, uint32_t audioBufferSize, uint32_t frequency) {
	soundStructure->frequency = frequency;
	soundStructure->soundBufferSize = audioBufferSize;

	for (uint32_t iterator = 0; iterator < audioBufferSize; iterator++) {
		soundStructure->soundBuffer[iterator] = audioBuffer[iterator];
	}
}

/**
 * @brief This function updates the sound buffer using "small" sound package of sound mail.
 * @param soundBuffer: pointer to SoundBuffer (destination)
 * @param SoundMail: pointer to SoundMail (source)
 */
void audioRecordingUpdateSoundBuffer(SoundBufferStr* soundBuffer,
		SoundMailStr* soundMail) {
	soundBuffer->frequency = soundMail->frequency;

	for (uint32_t i = 0; i < soundMail->soundBufferSize; i++) {
		soundBuffer->iterator++;
		if (soundBuffer->iterator >= soundBuffer->size)
			soundBuffer->iterator = 0;
		soundBuffer->soundBuffer[soundBuffer->iterator] =
				soundMail->soundBuffer[i];
	}
}

