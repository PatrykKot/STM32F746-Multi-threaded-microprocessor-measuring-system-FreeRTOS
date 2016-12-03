/*
 * jsonConfiguration.c
 *
 *  Created on: 5 paŸ 2016
 *      Author: Patryk Kotlarz
 */

#include "jsonConfiguration.h"

/**
 * @brief Parses \netbuf (JSON data) to \StmConfig structure
 */
void parseJSON(struct netbuf* buf, StmConfig* config) {
	void* data;
	uint16_t length;
	netbuf_data(buf, &data, &length);
	char* jsonData = (char*) data;

	cJSON* parser = cJSON_Parse(jsonData);
	/*config->started = cJSON_GetObjectItem(parser, "Started")->valueint;
	 char* udpEndpoint =
	 cJSON_GetObjectItem(parser, "UdpEndpointIP")->valuestring;
	 config->udpEndpointPort =
	 cJSON_GetObjectItem(parser, "UdpEndpointPort")->valueint;*/
	config->amplitudeSamplingDelay = cJSON_GetObjectItem(parser,
			"AmplitudeSamplingDelay")->valueint;
	config->audioSamplingFrequency = cJSON_GetObjectItem(parser,
	 "SamplingFrequency")->valueint;

	 /*sscanf(udpEndpoint, "%d.%d.%d.%d", (int*)&(config->udpEndpointAddr[0]),
	 (int*)&(config->udpEndpointAddr[1]), (int*)&(config->udpEndpointAddr[2]),
	 (int*)&(config->udpEndpointAddr[3]));*/
	cJSON_Delete(parser);
}

/**
 * @brief Converts \ref StmConfig structure to JSON string
 * @param config: pointer to \ref StmConfig structure
 * @param str: pointer to output of the JSON string (must have allocated memory)
 */
void stmConfigToString(StmConfig* config, char* str) {
	cJSON *jsonCreator;
	jsonCreator = cJSON_CreateObject();
	/*cJSON_AddBoolToObject(jsonCreator, "Started", config->started);
	 cJSON_AddNumberToObject(jsonCreator, "UdpEndpointPort",
	 config->udpEndpointPort);
	 cJSON_AddNumberToObject(jsonCreator, "FrequencyResolution",
	 config->frequencyResolution);*/
	cJSON_AddNumberToObject(jsonCreator, "AmplitudeSamplingDelay",
			config->amplitudeSamplingDelay);

	/*char ip[15];
	 sprintf(ip, "%d.%d.%d.%d", config->udpEndpointAddr[0],
	 config->udpEndpointAddr[1], config->udpEndpointAddr[2],
	 config->udpEndpointAddr[3]);
	 cJSON_AddStringToObject(jsonCreator, "UdpEndpointIP", ip);*/

	strcpy(str, cJSON_Print(jsonCreator));
	cJSON_Delete(jsonCreator);
}

/**
 * @brief Copies \ref StmConfig structure to another \ref StmConfig structure
 * @param destination: pointer (output) to \ref StmConfig structure
 * @param source: pointer to \ref StmConfig structure
 */
void copyConfig(StmConfig* destination, StmConfig* source) {
	destination->amplitudeSamplingDelay = source->amplitudeSamplingDelay;
	destination->audioSamplingFrequency = source->audioSamplingFrequency;

	/*destination->started = source->started;
	 for (uint16_t i = 0; i < 4; i++)
	 destination->udpEndpointAddr[i] = source->udpEndpointAddr[i];
	 destination->udpEndpointPort = source->udpEndpointPort;*/
}
