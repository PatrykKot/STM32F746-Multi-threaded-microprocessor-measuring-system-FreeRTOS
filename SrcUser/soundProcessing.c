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
	uint32_t i;
	uint32_t soundBuffIterator;
	spectrumStr->frequencyResolution = (float32_t) soundBuffer->frequency
			/ soundBuffer->size * 2;
	spectrumStr->vectorSize = soundBuffer->size / 2;

	soundBuffIterator = soundBuffer->iterator + 1;
	for (i = 0; i < soundBuffer->size; i++) {
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
	uint32_t i;
	SingleFreqStr freq;
	float32_t result = amplitudeStr->amplitudeVector[from];
	uint32_t index = from;

	for (i = from; i < to; i++) {
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
	uint32_t i;
	destination->frequencyResolution = source->frequencyResolution;
	destination->vectorSize = source->vectorSize;

	for (i = 0; i < destination->vectorSize; i++) {
		destination->amplitudeVector[i] = source->amplitudeVector[i];
	}
}

/**
 * @brief Calculates Hann window value
 * @param index: sample indes
 * @param length: all sample length
 * @retval value of Hann window
 */
float32_t calcHann(uint32_t index, uint32_t length)
{
	return (float32_t)0.5*((float32_t)1-arm_cos_f32((float32_t)(2*PI*index)/(float32_t)(length-1)));
}

static const float32_t flatTopTable[] = {0.21557895, 0.41663158, 0.277263158, 0.083578947, 0.006947368};

/**
 * @brief Calculates Flat top window value
 * @param index: sample indes
 * @param length: all sample length
 * @retval value of Flat top window
 */
float32_t calcFlatTop(uint32_t index, uint32_t length)
{
	return flatTopTable[0] - flatTopTable[1] * arm_cos_f32((float32_t)(2*PI*index)/(float32_t)(length-1)) + flatTopTable[2] * arm_cos_f32((float32_t)(4*PI*index)/(float32_t)(length-1)) - flatTopTable[3] * arm_cos_f32((float32_t)(6*PI*index)/(float32_t)(length-1)) + flatTopTable[4] * arm_cos_f32((float32_t)(8*PI*index)/(float32_t)(length-1));
}

/**
 * @brief Process audio signal by window
 * @param windowType: type of signal window
 * @param soundBuffer: pointer to audio buffer
 * @param length: audio buffer size
 */
void soundProcessingProcessWindow(WindowType windowType, float32_t* soundBuffer, uint32_t length)
{
	uint32_t index;
	
	switch(windowType)
	{
		case RECTANGLE:
		{
			break;
		}
		case HANN:
		{
			for(index = 0; index < length; index++)
			{
				soundBuffer[index] *= calcHann(index, length);
			}
			break;
		}
		case FLAT_TOP:
		{
			for(index = 0; index < length; index++)
			{
				soundBuffer[index] *= calcFlatTop(index, length);
			}
			break;
		}
		default:
		{
			break;
		}
	}
}
