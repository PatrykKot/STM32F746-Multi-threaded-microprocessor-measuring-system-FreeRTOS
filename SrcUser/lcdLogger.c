/*
 * logger.c
 *
 *  Created on: Jul 5, 2016
 *      Author: Patryk Kotlarz
 */

#include "lcdLogger.h"

/**
 * @val int row
 * @brief The next row index to show the log message
 */
static int row = 0;

/**
 * @val char* fullMsg
 * @brief Allocated memory to hold the message
 */
static char* fullMsg;

/**
 * @val int flag
 * @brief Equals 0 if the \ref fullMsg is not allocated
 */
static int flag = 0;

/**
 * @brief Displays the \p msg
 * @param msg: text message
 * @param col: font color
 */
static void disp(char* msg, uint32_t col) {
	BSP_LCD_SetTextColor(col);
	BSP_LCD_DisplayStringAtLine(row++, (uint8_t*) msg);
}

/**
 * @brief Checks if the row is too large
 */
static void updateRow() {
	if (row > LOG_MAX_ROWS) {
		row = 0;
		BSP_LCD_Clear(LOG_BACKGROUND_COLOR);
	}
}

/**
 * @brief Initializes LCD logging feature. At first initializes and clears the LCD. Then allocates memory for \ref fullMsg.
 */
void lcdInit() {
	BSP_LCD_Init();
	BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
	BSP_LCD_SelectLayer(0);
	BSP_LCD_DisplayOn();
	BSP_LCD_Clear(LOG_BACKGROUND_COLOR);
	BSP_LCD_SetTextColor(LOG_FONT_COLOR);
	BSP_LCD_SetFont(LOG_FONT);

	row = 0;
	if (flag == 0) {
		fullMsg = malloc(LOG_MAX_MESSAGE_LENGTH);
		flag = 1;
	}
}

/**
 * @brief The function displays info \p msg on the LCD in the next row.
 * @param msg: log message
 */
void logMsg(char* msg) {
	updateRow();
	disp(msg, LOG_FONT_COLOR);
}

/**
 * @brief The function displays error \p msg on the LCD in the next row.
 * @param msg: log message
 */
void logErr(char* msg) {
	updateRow();
	disp(msg, LCD_COLOR_RED);
}

/**
 * @brief The function displays info \p msg concatenated with \val number on the LCD in the next row .
 * @param msg: log message
 * @param val: value to concatenate
 */
void logMsgVal(char* msg, int val) {
	updateRow();
	sprintf(fullMsg, "%s%d", msg, val);
	disp(fullMsg, LOG_FONT_COLOR);
}

/**
 * @brief The function displays error \p msg concatenated with \val number on the LCD in the next row .
 * @param msg: log message
 * @param val: value to concatenate
 */
void logErrVal(char* msg, int val) {
	updateRow();
	sprintf(fullMsg, "%s%d", msg, val);
	disp(fullMsg, LCD_COLOR_RED);
}

/**
 * @brief The function displays info \p msg concatenated with \val floating point number on the LCD in the next row .
 * @param msg: log message
 * @param val: value to concatenate
 */
void logMsgValFt(char* msg, float val) {
	sprintf(fullMsg, "%s%5.2f", msg, val);
	disp(fullMsg, LOG_FONT_COLOR);
}

/**
 * @brief The function displays error \p msg concatenated with \val floating point number on the LCD in the next row .
 * @param msg: log message
 * @param val: value to concatenate
 */
void logErrValFt(char* msg, float val) {
	sprintf(fullMsg, "%s%5.2f", msg, val);
	disp(fullMsg, LCD_COLOR_RED);
}

/**
 * @brief The functions clears the LCD and resets \ref row
 */
void logClear() {
	row = 0;
	BSP_LCD_Clear(LOG_BACKGROUND_COLOR);
}
