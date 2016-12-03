/*
 * soundProcessing.c
 *
 *  Created on: 3 wrz 2016
 *      Author: Patryk Kotlarz
 */

#include "soundProcessing.h"

/**
 * @brief The function calculates the amplitude vector \p amplitudeStr using CMSIS DSP library
 * @param cfft_instance: pointer to \ref arm_cfft_instance_f32
 * @param amplitudeStr: pointer to \ref SpectrumStr - destination of amplitude vector
 * @param sourceBuffer: source buffer of audio samples
 */
void soundProcessingGetAmplitudeInstance(arm_cfft_instance_f32* cfft_instance,
		SpectrumStr* amplitudeStr, float32_t* sourceBuffer) {
	arm_cfft_f32(cfft_instance, sourceBuffer, 0, 1);
	arm_cmplx_mag_f32(sourceBuffer, amplitudeStr->amplitudeVector,
			cfft_instance->fftLen);
}

/**
 * @brief The function initializes \p amplitudeStr (sets the frequency resoultion and amplitude vector size) amd copies sound samples to \p destinationBuffer
 * @param spectrumStr: pointer to \ref SpectrumStr (destination)
 * @param soundBuffer: pointer to \ref SoundBuffer (source)
 * @param destinationBuffer: buffer to temporary hold the audio samples (destination)
 */
void soundProcessingAmplitudeInit(SpectrumStr* spectrumStr,
		SoundBufferStr* soundBuffer, float32_t* destinationBuffer) {
	spectrumStr->frequencyResolution = (float32_t) soundBuffer->frequency
			/ soundBuffer->size * 2;
	spectrumStr->vectorSize = soundBuffer->size / 2;

	uint32_t soundBuffIterator = soundBuffer->iterator + 1;
	for (uint32_t i = 0; i < soundBuffer->size; i++) {
		destinationBuffer[i] = soundBuffer->soundBuffer[soundBuffIterator++];
		if (soundBuffIterator >= soundBuffer->size)
			soundBuffIterator = 0;
	}
}

/**
 * @brief Returns the \ref SingleFreqStr instance which is representating the frequency with the maximum amplitude found in the amplitude vector
 * @param amplitudeStr: pointer to \ref SpectrumStr
 * @param from: lowest index
 * @param to: highest index
 *
 * The index range was used because always the first element in the amplitude vector has the highest value.
 * @retval \ref SingleFreq instance
 */
SingleFreqStr soundProcessingGetStrongestFrequency(SpectrumStr* amplitudeStr,
		uint32_t from, uint32_t to) {
	SingleFreqStr freq;
	float32_t result = amplitudeStr->amplitudeVector[from];
	uint32_t index = from;

	for (uint32_t i = from; i < to; i++) {
		if (amplitudeStr->amplitudeVector[i] > result) {
			result = amplitudeStr->amplitudeVector[i];
			index = i;
		}
	}

	freq.amplitude = result;
	freq.frequency = (float32_t) index * amplitudeStr->frequencyResolution;

	return freq;
}

/**
 * @brief Matches correct \ref arm_cfft_instance_f32 with the specified \p length.
 * @param instance: pointer to \ref arm_cfft_instance_f32 structure
 * @param length: \ref arm_cfft_instance_f32 length
 * The \p length can be only the power of two (from 16 to 4096)
 */
void soundProcessingGetCfftInstance(arm_cfft_instance_f32* instance,
		uint32_t length) {
	switch (length) {
	case 16: {
		*instance = arm_cfft_sR_f32_len16;
		break;
	}
	case 32: {
		*instance = arm_cfft_sR_f32_len32;
		break;
	}
	case 64: {
		*instance = arm_cfft_sR_f32_len64;
		break;
	}
	case 128: {
		*instance = arm_cfft_sR_f32_len128;
		break;
	}
	case 256: {
		*instance = arm_cfft_sR_f32_len256;
		break;
	}
	case 512: {
		*instance = arm_cfft_sR_f32_len512;
		break;
	}
	case 1024: {
		*instance = arm_cfft_sR_f32_len1024;
		break;
	}
	case 2048: {
		*instance = arm_cfft_sR_f32_len2048;
		break;
	}
	case 4096: {
		*instance = arm_cfft_sR_f32_len4096;
		break;
	}
	default: {
		instance = NULL;
		break;
	}
	}
}

/**
 * @brief Copies \ref SpectrumStr structure to another \red SpectrumStr structure
 * @param source: pointer to \ref SpectrumStr structure
 * @param destination: pointer (output) to \ref SpectrumStr structure
 */
void soundProcessingCopyAmplitudeInstance(SpectrumStr* source,
		SpectrumStr* destination) {
	destination->frequencyResolution = source->frequencyResolution;
	destination->vectorSize = source->vectorSize;

	for (uint32_t i = 0; i < destination->vectorSize; i++) {
		destination->amplitudeVector[i] = source->amplitudeVector[i];
	}
}
