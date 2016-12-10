/*
 * jsonConfiguration.h
 *
 *  Created on: 5 paŸ 2016
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

#define IP_ADDR_GET(ipaddr,index) (int)(((u32_t)(ipaddr.addr)>>((u32_t)(8*index)))&((u32_t)0xff))

#define TRUE 1
#define FALSE 0

/**
 * @brief Structure represents device configuration
 */
typedef struct {
	//uint8_t started;
	//uint8_t udpEndpointAddr[4];
	//uint32_t udpEndpointPort;
	uint8_t amplitudeSamplingDelay;
	uint32_t audioSamplingFrequency;
	ip_addr_t clientIp;
	uint32_t clientPort;
} StmConfig;

/* Functions */
void parseJSON(struct netbuf* buf, StmConfig* config);
void stmConfigToString(StmConfig* config, char* str);
void copyConfig(StmConfig* destination, StmConfig* source);

#endif /* JSONCONFIGURATION_H_ */

