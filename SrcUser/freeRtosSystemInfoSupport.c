/*
 * freeRtosSystemInfoSupport.c
 *
 *  Created on: 1 sty 2017
 *      Author: Patryk
 */

#include "freeRtosSystemInfoSupport.h"

extern uint16_t tim6OverflowCount;

void getTaskUsageDetails(char* jsonData) {
	char detailsStr[256];
	vTaskGetRunTimeStats(detailsStr);
	parseTaskUsage(detailsStr, jsonData);
}

void configureTimerForRuntimestats() {
	MX_TIM11_Init();
}

uint32_t getTimVal() {
	return ((tim6OverflowCount << 16) | TIM11->CNT);
}

void parseTaskUsage(char* detailsStr, char* jsonData) {

	cJSON *jsonCreator;
	jsonCreator = cJSON_CreateArray();
	cJSON *arrayObject;

	uint32_t length = strlen(detailsStr);
	uint32_t iterator = 0;
	uint32_t countIterator = 0;

	char tempStr[32];
	uint32_t tempIterator = 0;

	while (iterator < length) {
		cJSON_AddItemToArray(jsonCreator, arrayObject = cJSON_CreateObject());

		ignoreWhitespace(&iterator, detailsStr);

		// Parsing taskname
		tempIterator = 0;
		while (!isWhitespace(detailsStr[iterator]) && iterator < length) {
			tempStr[tempIterator++] = detailsStr[iterator++];
		}
		tempStr[tempIterator] = '\0';
		cJSON_AddStringToObject(arrayObject, "taskName", tempStr);

		ignoreWhitespace(&iterator, detailsStr);

		// Parsing memory
		tempIterator = 0;
		while (isDigit(detailsStr[iterator]) && iterator < length) {
			tempStr[tempIterator++] = detailsStr[iterator++];
		}
		tempStr[tempIterator] = '\0';
		uint32_t memoryUsage = atoi(tempStr);

		cJSON_AddNumberToObject(arrayObject, "memory", memoryUsage);

		ignoreWhitespace(&iterator, detailsStr);

		// Parsing usage
		tempIterator = 0;
		while (!isWhitespace(detailsStr[iterator]) && iterator < length) {
			tempStr[tempIterator++] = detailsStr[iterator++];
		}
		tempStr[tempIterator] = '\0';

		cJSON_AddStringToObject(arrayObject, "usage", tempStr);

		ignoreWhitespace(&iterator, detailsStr);
		countIterator++;
	}

	char* json = cJSON_Print(jsonCreator);
	strcpy(jsonData, json);
	cJSON_Delete(jsonCreator);
	free(json);
}

void ignoreWhitespace(uint32_t* iterator, char* str) {
	uint32_t length = strlen(str);
	while (isWhitespace(str[*iterator]) && *iterator < length) {
		*iterator = *iterator + 1;
	}
}

uint32_t countNumberOfLines(char* str) {
	uint32_t len = strlen(str);
	uint32_t count = 0;

	for (uint32_t i = 0; i < len; i++) {
		if (str[i] == '\n') {
			count++;
		}
	}

	return count;
}

uint8_t isDigit(char character) {
	return character >= '0' && character <= '9';
}

uint8_t isWhitespace(char character) {
	return character == ' ' || character == '\n' || character == '\r'
			|| character == '\t';
}

uint8_t isPercent(char character) {
	return character == '%';
}

uint8_t isLessThan(char character) {
	return character == '<';
}

