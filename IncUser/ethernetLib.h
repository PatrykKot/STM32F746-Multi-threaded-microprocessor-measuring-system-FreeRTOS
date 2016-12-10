/*
 * ethernetLib.h
 *
 *  Created on: 3 wrz 2016
 *      Author: Patryk Kotlarz
 */

#ifndef ETHERNETLIB_H_
#define ETHERNETLIB_H_

#include "stdint.h"
#include "netif.h"
#include "lcdLogger.h"
#include "stm32f7xx_hal_eth.h"
#include "soundProcessing.h"
#include "lwip.h"
#include "jsonConfiguration.h"

/**
 * @def MAX_DHCP_TRIES
 * @brief The number of maximum DHCP connection tries
 */
#define MAX_DHCP_TRIES  2

/*
 * DHCP states
 */
#define DHCP_START                 (uint8_t) 1
#define DHCP_WAIT_ADDRESS          (uint8_t) 2
#define DHCP_ADDRESS_ASSIGNED      (uint8_t) 3
#define DHCP_TIMEOUT               (uint8_t) 4
#define DHCP_LINK_DOWN             (uint8_t) 5

/*
 * Static IP address of STM device if the LWIP cannot find the DHCP server
 */
#define IP_ADDR0 192
#define IP_ADDR1 168
#define IP_ADDR2 1
#define IP_ADDR3 11

/*
 * Static netmask address of STM device if the LWIP cannot find the DHCP server
 */
#define NETMASK_ADDR0 255
#define NETMASK_ADDR1 255
#define NETMASK_ADDR2 255
#define NETMASK_ADDR3 0

/*
 * Static gateway address of STM device if the LWIP cannot find the DHCP server
 */
#define GW_ADDR0 192
#define GW_ADDR1 168
#define GW_ADDR2 1
#define GW_ADDR3 1

/*
 * Used to recognize type of address (only for error logging)
 */
#define IP_ADDRESS 0
#define NETMASK_ADDRESS 1
#define GATEWAY_ADDRESS 2

/*
 * State of ethernet cable
 */
#define ETHERNET_CABLE_NOT_CONNECTED 0
#define ETHERNET_CABLE_CONNECTED 1

/**
 * @def ETHERNET_AMP_BUFFER_SIZE
 * @brief Amplitude data length sent to PC
 */
#define ETHERNET_AMP_BUFFER_SIZE 256

/**
 * HTTP request types
 */
#define NOT_SUPPORTED_REQUEST 0
#define GET_REQUEST 1
#define PUT_REQUEST 2

/**
 * UDP streaming port
 */
#define UDP_STREAMING_PORT 53426

/* Functions */
void printAddress(const struct netif* gnetif, uint8_t addressType);
uint32_t isEthernetCableConnected();
err_t sendSpectrum(SpectrumStr* ampStr, struct netconn *client);
uint8_t isNetconnStatusOk(err_t status);
err_t udpSend(struct netconn *client, void* buf, uint32_t buffSize);
void printContent(struct netbuf* buf);
uint16_t getRequestType(struct netbuf* buf);
err_t sendConfiguration(StmConfig* config, struct netconn* client,
		char* requestParameters);
err_t sendHttpResponse(struct netconn* client, char* httpStatus,
		char* requestParameters, char* content);
err_t sendString(struct netconn* client, const char* array);
uint8_t isConfigRequest(struct netbuf* buf);

#endif /* ETHERNETLIB_H_ */
