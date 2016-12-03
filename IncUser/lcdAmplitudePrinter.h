/*
 * lcdAmplitudePrinter.h
 *
 *  Created on: 8 wrz 2016
 *      Author: Patryk Kotlarz
 */

#ifndef LCDAMPLITUDEPRINTER_H_
#define LCDAMPLITUDEPRINTER_H_

#include "soundProcessing.h"
#include "stm32746g_discovery_lcd.h"
#include "lcdLogger.h"

//#define LCD_PRINTER_SUPPORT

/**
 * @def LCD_WIDTH
 */
#define LCD_WIDTH 480

/**
 * @def LCD_HEIGHT
 */
#define LCD_HEIGHT 272

/* Functions */
void lcdAmpPrinterPrint(SpectrumStr* amp);

#endif /* LCDAMPLITUDEPRINTER_H_ */
