/*
 * jsonConfiguration.c
 *
 *  Created on: 5 paz 2016
 *      Author: Patryk Kotlarz
 */

#include "jsonConfiguration.h"

/**
 * @brief Parses JSON data to \StmConfig structure
 */
void parseJSON(char* jsonData, StmConfig* config) {
	char windowTypeStr[20];
	char errorMsg[35];
	cJSON* parser;

	config->amplitudeSamplingDelay = 0;
	config->audioSamplingFrequency = 0;
	strcpy(config->clientIp, "");
	config->clientPort = 0;
	config->windowType = UNDEFINED;

	parser = cJSON_Parse(jsonData);
	if(!parser)
	{
		sprintf(errorMsg, "cJSON error");
		logErr(errorMsg);
		return;
	}

	if(cJSON_HasObjectItem(parser,"UdpEndpointIP"))
	{
		strcpy(config->clientIp, cJSON_GetObjectItem(parser, "UdpEndpointIP")->valuestring);
	}

	if(cJSON_HasObjectItem(parser,"AmplitudeSamplingDelay"))
	{
		config->amplitudeSamplingDelay = cJSON_GetObjectItem(parser, "AmplitudeSamplingDelay")->valueint;
	}

	if(cJSON_HasObjectItem(parser,"SamplingFrequency"))
	{
		config->audioSamplingFrequency = cJSON_GetObjectItem(parser, "SamplingFrequency")->valueint;
	}

	if(cJSON_HasObjectItem(parser,"UdpEndpointPort"))
	{
		config->clientPort = cJSON_GetObjectItem(parser, "UdpEndpointPort")->valueint;
	}

	if(cJSON_HasObjectItem(parser,"WindowType"))
	{
		strcpy(windowTypeStr, cJSON_GetObjectItem(parser, "WindowType")->valuestring);

		if(strcmp(windowTypeStr, "RECTANGLE") == 0)
		{
			config->windowType = RECTANGLE;
		}
		else if (strcmp(windowTypeStr, "HANN") == 0)
		{
			config->windowType = HANN;
		}
		else if(strcmp(windowTypeStr, "FLAT_TOP") == 0)
		{
			config->windowType = FLAT_TOP;
		}
	}

	cJSON_Delete(parser);
}

/**
 * @brief Converts \ref StmConfig structure to JSON string
 * @param config: pointer to \ref StmConfig structure
 * @param str: pointer to output of the JSON string (must have allocated memory)
 */
void stmConfigToString(StmConfig* config, char* str, uint32_t len) {
	char windowTypeStr[20];
	cJSON *jsonCreator;

	switch(config->windowType)
	{
		case RECTANGLE:
		{
			strcpy(windowTypeStr, "RECTANGLE");
			break;
		}
		case HANN:
		{
			strcpy(windowTypeStr, "HANN");
			break;
		}
		case FLAT_TOP:
		{
			strcpy(windowTypeStr, "FLAT_TOP");
			break;
		}
		default:
		{
			strcpy(windowTypeStr, "UNDEFINED");
			break;
		}
	}

	jsonCreator = cJSON_CreateObject();
	cJSON_AddNumberToObject(jsonCreator, "UdpEndpointPort", config->clientPort);
	cJSON_AddNumberToObject(jsonCreator, "AmplitudeSamplingDelay",
			config->amplitudeSamplingDelay);
	cJSON_AddNumberToObject(jsonCreator, "SamplingFrequency",
			config->audioSamplingFrequency);
	cJSON_AddStringToObject(jsonCreator, "UdpEndpointIP", config->clientIp);
	cJSON_AddStringToObject(jsonCreator, "WindowType", windowTypeStr);

	cJSON_PrintPreallocated(jsonCreator, str, len, FALSE);
	cJSON_Delete(jsonCreator);
}

/**
 * @brief Copies \ref StmConfig structure to another \ref StmConfig structure
 * @param destination: pointer (output) to \ref StmConfig structure
 * @param source: pointer to \ref StmConfig structure
 */
void copyConfig(StmConfig* destination, StmConfig* source) {
	if(source->amplitudeSamplingDelay != 0)
	{
		destination->amplitudeSamplingDelay = source->amplitudeSamplingDelay;
	}

	if(source->audioSamplingFrequency != 0)
	{
		destination->audioSamplingFrequency = source->audioSamplingFrequency;
	}

	if(strlen(source->clientIp)>0)
	{
		strcpy(destination->clientIp, source->clientIp);
	}

	if(source->clientPort != 0)
	{
		destination->clientPort = source->clientPort;
	}

	if(source->windowType > UNDEFINED && source->windowType <= FLAT_TOP)
	{
		destination->windowType = source->windowType;
	}

}

void makeChanges(StmConfig* newConfig, StmConfig* oldConfig) {
	char msg[30];

	if(newConfig->amplitudeSamplingDelay != oldConfig->amplitudeSamplingDelay && newConfig->amplitudeSamplingDelay != 0)
	{
		logMsgVal("Changed sampling delay ", newConfig->amplitudeSamplingDelay);
	}

	if(newConfig->audioSamplingFrequency != oldConfig->audioSamplingFrequency && newConfig->audioSamplingFrequency != 0)
	{
		audioRecorderSetSamplingFrequency(newConfig->audioSamplingFrequency);
		logMsgVal("Changed sampling frequency ", newConfig->audioSamplingFrequency);
	}

	if(newConfig->clientPort != oldConfig->clientPort && newConfig->clientPort != 0)
	{
		logMsgVal("Changed client port ", newConfig->clientPort);
	}

	if(strcmp(newConfig->clientIp, oldConfig->clientIp) && strlen(newConfig->clientIp) > 0)
	{
		sprintf(msg, "Changed client IP: %s", newConfig->clientIp);
		logMsg(msg);
	}

	if(newConfig->windowType != oldConfig->windowType && newConfig->windowType > UNDEFINED && newConfig->windowType <= HANN)
	{
		switch(newConfig->windowType)
		{
			case RECTANGLE:
			{
				logMsg("Changed window RECTANGLE");
				break;
			}
			case HANN:
			{
				logMsg("Changed window HANN");
				break;
			}
			case FLAT_TOP:
			{
				logMsg("Changed window FLAT_TOP");
				break;
			}
			default:
			{
				logErr("Unknown window");
				break;
			}
		}
	}
}
