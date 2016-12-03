/*
 * soundProcessing.h
 *
 *  Created on: 3 wrz 2016
 *      Author: Patryk Kotlarz
 */

#ifndef SOUNDPROCESSING_H_
#define SOUNDPROCESSING_H_

#include "audioRecording.h"
#include "arm_math.h"
#include "arm_const_structs.h"


/**
 * @def AMPLITUDE_STR_MAX_BUFFER_SIZE
 * @brief Maximum bufffer size of the amplitude structure \ref AmplitudeStr
 */
#define AMPLITUDE_STR_MAX_BUFFER_SIZE MAIN_SOUND_BUFFER_MAX_BUFFER_SIZE/2+1

/**
 * @brief SpectrumStr structure (amplitude data)
 */
typedef struct {
	float32_t amplitudeVector[AMPLITUDE_STR_MAX_BUFFER_SIZE];
	uint32_t vectorSize;
	float32_t frequencyResolution;
} SpectrumStr;

/**
 * @brief SingleFreqStr structure (amplitude value and frequency)
 */
typedef struct {
	float32_t amplitude;
	float32_t frequency;
} SingleFreqStr;

/* Functions */
void soundProcessingGetAmplitudeInstance(arm_cfft_instance_f32* cfft_instance,
		SpectrumStr* amplitudeStr, float32_t* sourceBuffer);
void soundProcessingAmplitudeInit(SpectrumStr* amplitudeStr,
		SoundBufferStr* soundBuffer, float32_t* destinationBuffer);
SingleFreqStr soundProcessingGetStrongestFrequency(SpectrumStr* amplitudeStr,
		uint32_t from, uint32_t to);
void soundProcessingGetCfftInstance(arm_cfft_instance_f32* instance,
		uint32_t length);
void soundProcessingCopyAmplitudeInstance(SpectrumStr* source,
		SpectrumStr* destination);

#endif /* SOUNDPROCESSING_H_ */
