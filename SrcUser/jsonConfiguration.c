/*
 * jsonConfiguration.c
 *
 *  Created on: 5 paŸ 2016
 *      Author: Patryk
 */

#include "jsonConfiguration.h"

void parse(struct netbuf* buf, StmConfig* config) {
	void* data;
	uint16_t length;
	netbuf_data(buf, &data, &length);
	char* jsonData = (char*) data;

	cJSON* parser = cJSON_Parse(jsonData);
	config->started = cJSON_GetObjectItem(parser, "Started")->valueint;
	char* udpEndpoint =
			cJSON_GetObjectItem(parser, "UdpEndpointIP")->valuestring;
	config->udpEndpointPort =
			cJSON_GetObjectItem(parser, "UdpEndpointPort")->valueint;
	config->amplitudeSamplingDelay = cJSON_GetObjectItem(parser,
			"AmplitudeSamplingDelay")->valueint;
	config->frequencyResolution = cJSON_GetObjectItem(parser,
			"FrequencyResolution")->valuedouble;

	sscanf(udpEndpoint, "%d.%d.%d.%d", &(config->udpEndpointAddr[0]),
			&(config->udpEndpointAddr[1]), &(config->udpEndpointAddr[2]),
			&(config->udpEndpointAddr[3]));
	cJSON_Delete(parser);
}

void stmConfigToString(StmConfig* config, char* str) {
	cJSON *jsonCreator;
	jsonCreator = cJSON_CreateObject();
	cJSON_AddBoolToObject(jsonCreator, "Started", config->started);
	cJSON_AddNumberToObject(jsonCreator, "UdpEndpointPort",
			config->udpEndpointPort);
	cJSON_AddNumberToObject(jsonCreator, "FrequencyResolution",
			config->frequencyResolution);
	cJSON_AddNumberToObject(jsonCreator, "AmplitudeSamplingDelay",
			config->amplitudeSamplingDelay);

	char ip[15];
	sprintf(ip, "%d.%d.%d.%d", config->udpEndpointAddr[0],
			config->udpEndpointAddr[1], config->udpEndpointAddr[2],
			config->udpEndpointAddr[3]);
	cJSON_AddStringToObject(jsonCreator, "UdpEndpointIP", ip);

	strcpy(str, cJSON_Print(jsonCreator));
	cJSON_Delete(jsonCreator);
}

void configCopy(StmConfig* destination, StmConfig* source) {
	destination->amplitudeSamplingDelay = source->amplitudeSamplingDelay;
	destination->started = source->started;
	for (uint16_t i = 0; i < 4; i++)
		destination->udpEndpointAddr[i] = source->udpEndpointAddr[i];
	destination->udpEndpointPort = source->udpEndpointPort;
}
