/*
 * freeRtosSystemInfoSupport.h
 *
 *  Created on: 1 sty 2017
 *      Author: Patryk
 */

#ifndef FREERTOSSYSTEMINFOSUPPORT_H_
#define FREERTOSSYSTEMINFOSUPPORT_H_

#include "stdint.h"
#include "FreeRTOS.h"
#include "task.h"
#include "mcuConfig.h"
#include "cJSON.h"
#include "string.h"

typedef struct {
	char taskName[20];
	uint32_t memory;
	char usage[10];
} TaskUsage;

void getTaskUsageDetails(char* jsonData);
uint32_t getTimVal();
void parseTaskUsage(char* detailsStr, char* jsonData);
void ignoreWhitespace(uint32_t* iterator, char* str);
uint32_t countNumberOfLines(char* str);
uint8_t isDigit(char character);
uint8_t isWhitespace(char character);
uint8_t isPercent(char character);
uint8_t isLessThan(char character);

#endif /* FREERTOSSYSTEMINFOSUPPORT_H_ */
