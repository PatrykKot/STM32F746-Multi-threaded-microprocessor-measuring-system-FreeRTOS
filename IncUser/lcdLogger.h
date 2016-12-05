/*
 * logger.h
 *
 *  Created on: Jul 5, 2016
 *      Author: Patryk Kotlarz
 */

#ifndef LCDLOGGER_H_
#define LCDLOGGER_H_

#include "stm32746g_discovery_lcd.h"
#include "stdlib.h"
#include "arm_math.h"
#include "jsonConfiguration.h"

/**
 * @def LOG_MAX_ROWS
 */
#define LOG_MAX_ROWS 22

/**
 * @def LOG_MAX_MESSAGE_LENGTH
 */
#define LOG_MAX_MESSAGE_LENGTH 50

/**
 * @def LOG_FONT
 */
#define LOG_FONT &Font12

/**
 * @def LOG_BACKGROUND_COLOR
 */
#define LOG_BACKGROUND_COLOR LCD_COLOR_WHITE

/**
 * @def LOG_FONT_COLOR
 */
#define LOG_FONT_COLOR LCD_COLOR_BLACK

/* Functions */
void lcdInit();
void logMsg(char* msg);
void logErr(char* mag);
void logMsgVal(char* msg, int val);
void logErrVal(char* msg, int val);
void logMsgValFt(char* msg, float val);
void logErrValFt(char* msg, float val);
void logClear();

#endif /* LCDLOGGER_H_ */
