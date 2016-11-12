/*
 * soundProcessing.h
 *
 *  Created on: 3 wrz 2016
 *      Author: Patryk
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
#define AMPLITUDE_STR_MAX_BUFFER_SIZE SOUND_BUFFER_MAX_BUFFER_SIZE/2+1

/**
 * AmplitudeStr structure (amplitude data)
 */
typedef struct {
	float32_t amplitudeVector[AMPLITUDE_STR_MAX_BUFFER_SIZE];
	uint32_t vectorSize;
	float32_t frequencyResolution;
} AmplitudeStr;

/**
 * SingleFreq structure (amplitude value and frequency)
 */
typedef struct {
	float32_t amplitudeVal;
	float32_t frequency;
} SingleFreq;

/* Functions */
void soundProcessingGetAmplitudeInstance(arm_cfft_instance_f32* cfft_instance,
		AmplitudeStr* amplitudeStr, float32_t* sourceBuffer);
void soundProcessingAmplitudeInit(AmplitudeStr* amplitudeStr,
		SoundBuffer* soundBuffer, float32_t* destinationBuffer);
SingleFreq soundProcessingGetStrongestFrequency(AmplitudeStr* amplitudeStr,
		uint32_t from, uint32_t to);
void soundProcessingGetCfftInstance(arm_cfft_instance_f32* instance,
		uint32_t length);
void soundProcessingCopyAmplitudeInstance(AmplitudeStr* source,
		AmplitudeStr* destination);

#endif /* SOUNDPROCESSING_H_ */
