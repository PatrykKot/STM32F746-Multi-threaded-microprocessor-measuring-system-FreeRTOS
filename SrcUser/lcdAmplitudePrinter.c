/*
 * lcdAmplitudePrinter.c
 *
 *  Created on: 8 wrz 2016
 *      Author: Patryk Kotlarz
 */

#include "lcdAmplitudePrinter.h"

/**
 * @brief The function prints the amplitude \p amp on the LCD screen.
 * @param amp: pointer to \ref SpectrumStr structure
 */
void lcdAmpPrinterPrint(SpectrumStr* amp) {

	uint16_t ampVal;
	uint16_t itCount = LCD_WIDTH;
	if (LCD_WIDTH > amp->vectorSize)
		itCount = amp->vectorSize;

	static float32_t maxAmp = 1;
	float32_t newMaxAmp = soundProcessingGetStrongestFrequency(amp, 1,
			itCount - 1).amplitude;
	if (newMaxAmp > maxAmp)
		maxAmp = newMaxAmp;

	for (uint16_t i = 0; i < itCount; i++) {
		ampVal = (maxAmp - amp->amplitudeVector[i]) / maxAmp * LCD_HEIGHT;
		if (ampVal > LCD_HEIGHT)
			ampVal = LCD_HEIGHT;

		for (uint32_t j = 0; j < LCD_HEIGHT; j++) {
			if (j < ampVal) {
				BSP_LCD_DrawPixel(i, j, LCD_COLOR_WHITE);
			} else {
				BSP_LCD_DrawPixel(i, j, LCD_COLOR_BLACK);
			}
		}
	}
}
