/*
 * jsonConfiguration.h
 *
 *  Created on: 5 paü 2016
 *      Author: Patryk Kotlarz
 */

#ifndef JSONCONFIGURATION_H_
#define JSONCONFIGURATION_H_

#include "stdint.h"
#include "arm_math.h"
#include "lwip.h"
#include "netif.h"
#include "lcdLogger.h"
#include "cJSON.h"

#define TRUE 1
#define FALSE 0

/**
 * @brief Structure represents device configuration
 */
typedef struct {
	uint8_t started;
	uint8_t udpEndpointAddr[4];
	uint32_t udpEndpointPort;
	uint8_t amplitudeSamplingDelay;
	float32_t frequencyResolution;
} StmConfig;

/* Functions */
void parse(struct netbuf* buf, StmConfig* config);
void stmConfigToString(StmConfig* config, char* str);
void configCopy(StmConfig* destination, StmConfig* source);

#endif /* JSONCONFIGURATION_H_ */
