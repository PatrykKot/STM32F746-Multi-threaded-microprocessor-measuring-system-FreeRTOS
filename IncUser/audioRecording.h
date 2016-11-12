/*
 * audioRecording.h
 *
 *  Created on: 3 wrz 2016
 *      Author: Patryk
 */

#ifndef AUDIORECORDING_H_
#define AUDIORECORDING_H_

#include "stm32746g_discovery_audio.h"
#include "stdlib.h"
#include "arm_math.h"
#include "lcdLogger.h"

/**
 * @def AUDIO_RECORDER_INPUT_MICROPHONE
 * @brief Available input devices addresses
 */
#define AUDIO_RECORDER_INPUT_MICROPHONE ((uint16_t)0x0200)
#define AUDIO_RECORDER_INPUT_LINE ((uint16_t)0x0300)

/**
 * @def AUDIO_RECORDER_OK
 * @brief No errors
 */
#define AUDIO_RECORDER_OK AUDIO_OK

/**
 * @def AUDIO_BUFFER_SIZE
 * @brief Audio buffer size
 */
#define AUDIO_BUFFER_SIZE 256


/**
 * @def AUDIO_RECORDER_VOLUME_0DB
 * @brief Audio volume for 0 dB gain
 */
#define AUDIO_RECORDER_VOLUME_0DB ((uint32_t)80)

/**
 * @def audioRecorder_FullBufferFilled
 * @brief The name of full buffer filled callback (function).
 */
#define audioRecorder_FullBufferFilled BSP_AUDIO_IN_TransferComplete_CallBack


/**
 * @def SOUND_MAIL_MAX_BUFFER_SIZE
 * @brief Audio buffer size in the \ref SoundMail structure
 */
#define SOUND_MAIL_MAX_BUFFER_SIZE AUDIO_BUFFER_SIZE

/**
 * Sound mail structure
 */
typedef struct {
	uint16_t soundBuffer[SOUND_MAIL_MAX_BUFFER_SIZE];
	uint32_t soundBufferSize;
	uint32_t frequency;
} SoundMail;


/**
 * @def SOUND_BUFFER_MAX_BUFFER_SIZE
 * @brief Audio buffeer size in \ref SoundBuffer structure
 */
#define SOUND_BUFFER_MAX_BUFFER_SIZE 4096
typedef struct {
	uint16_t soundBuffer[SOUND_BUFFER_MAX_BUFFER_SIZE];
	uint32_t soundBufferSize;
	uint32_t frequency;
	uint32_t iterator;
} SoundBuffer;

/* Functions */
uint8_t audioRecorderInit(uint16_t inputDevice, uint8_t volume,
		uint32_t audioFreq);
uint8_t audioRecorderStartRecording(uint16_t* audioBuffer,
		uint32_t audioBufferSize);
uint8_t audioRecorderSetVolume(uint8_t volume);

void audioRecordingUpdateSoundBuffer(SoundBuffer* soundBuffer,
		SoundMail* soundMail);
void audioRecordingSoundMailFill(SoundMail* soundStructure,
		uint16_t* audioBuffer, uint32_t audioBufferSize, uint32_t frequency);

#endif /* AUDIORECORDING_H_ */
