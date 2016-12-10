/*
 * ethernetLib.c
 *
 *  Created on: 3 wrz 2016
 *      Author: Patryk Kotlarz
 */

#include "ethernetLib.h"

/**
 * @var uint32_t DHCP_state
 * @brief State of DHCP server finding module
 */
uint32_t dhcpState;

/**
 * @var ETH_HandleTypeDef EthHandle
 * @brief Ethernet module handle
 */
extern ETH_HandleTypeDef EthHandle;

/**
 * @var char httpOkHeaderPattern[]
 * @brief Plain header of 200 HTTP response
 */
const char httpOkHeaderPattern[] =
		"HTTP/1.0 200 OK\r\nContent-Type: text/plain\r\nContent-Length: %d%s\r\n\r\n%s";
/**
 * @brief Used for printing the IP, netmask or gateway address
 * @param gnetif: pointer to \ref netif structure
 * @param addressType: type of address (IP_ADDRESS etc.)
 */
void printAddress(const struct netif* gnetif, uint8_t addressType) {
	char addrType[8];
	char msg[40];
	uint32_t address = 0;
	switch (addressType) {
	case IP_ADDRESS: {
		strcpy(addrType, "IP");
		address = gnetif->ip_addr.addr;
		break;
	}
	case NETMASK_ADDRESS: {
		strcpy(addrType, "Netmask");
		address = gnetif->netmask.addr;
		break;
	}
	case GATEWAY_ADDRESS: {
		strcpy(addrType, "Gateway");
		address = gnetif->gw.addr;
		break;
	}
	default:
		break;
	}
	sprintf(msg, "%s address: %d.%d.%d.%d", addrType, (uint8_t) (address),
			(uint8_t) (address >> 8), (uint8_t) (address >> 16),
			(uint8_t) (address >> 24));
	logMsg(msg);
}

/**
 * @brief The function checks state of ethernet cable (is it connected or not).
 * @retval \ref ETHERNET_CABLE_NOT_CONNECTED or \ref ETHERNET_CABLE_CONNECTED
 */
uint32_t isEthernetCableConnected() {
	uint32_t val;

	// reads from PHY register
	HAL_ETH_ReadPHYRegister(&EthHandle, 1, &val);
	val = val & (1 << 2);
	return val;
}

/**
 * @brief The function sends the \p ampStr by UDP to \p client.
 * @param ampStr: pointer to \ref AmplitudeStr
 * @param client: pointer to \ref netconn
 * @retval returns \ref ERR_OK if there are no errors
 */
err_t sendSpectrum(SpectrumStr* ampStr, struct netconn *client) {
	err_t status;

	if (client != NULL)
		if (client->state != NETCONN_CLOSE) {
			status = udpSend(client, ampStr->amplitudeVector,
			ETHERNET_AMP_BUFFER_SIZE * sizeof(float32_t));
			if (!isNetconnStatusOk(status))
				return status;
		}
	return ERR_OK;
}

/**
 * @brief The functions checks the returned \ref err_t because sometimes LWIP functions returns \ref ERR_RST if the ethernet cable is disconnected.
 * @param status: error code
 * @retval returns 0 if there are no errors
 */
uint8_t isNetconnStatusOk(err_t status) {
	if (status != ERR_OK && status != ERR_RST)
		return 0;
	return 1;
}

/**
 * @brief Used to send a some buffer \p buf to \p client by UDP
 * @param client: pointer to \ref netconn
 * @param buf: pointer to the beginning of data
 * @param buffSize: data length
 * @retval returns \ref ERR_OK if there are no errors
 */
err_t udpSend(struct netconn *client, void* buf, uint32_t buffSize) {
	err_t err;
	struct netbuf* netBuf = netbuf_new();
	err = netbuf_ref(netBuf, buf, buffSize);
	if (err != ERR_OK) {
		netbuf_delete(netBuf);
		return err;
	}
	err = netconn_send(client, netBuf);
	netbuf_delete(netBuf);
	return err;
}

void printContent(struct netbuf* buf) {
	void* data;
	uint16_t length;
	netbuf_data(buf, &data, &length);
	char* tok;
	char* fullMsg = (char*) data;
	tok = strtok((char*) fullMsg, "\n");
	while (tok != NULL) {
		logMsg(tok);
		tok = strtok(NULL, "\n");
	}
}

/**
 * @brief Returns the request type
 * @param buf: pointer to \ref netbuf structure
 * @retval GET_REQUEST, PUT_REQUEST or NOT_SUPPORTED_REQUEST
 */
uint16_t getRequestType(struct netbuf* buf) {
	void* data;
	uint16_t length;
	netbuf_data(buf, &data, &length);
	char* fullMsg = (char*) data;

	if (strstr(fullMsg, "GET") != NULL)
		return GET_REQUEST;
	else if (strstr(fullMsg, "PUT") != NULL)
		return PUT_REQUEST;
	else
		return NOT_SUPPORTED_REQUEST;
}

err_t sendData(struct netconn* client, char* requestParameters, char* data) {
	err_t netStatus;

	char configResponse[512];

	sprintf(configResponse, httpOkHeaderPattern, strlen(data),
			requestParameters, data);

	netStatus = sendString(client, configResponse);
	if (netStatus != ERR_OK)
		return netStatus;
	return netStatus;
}

/**
 * @brief Sens the device configuration to the client
 * @param config: pointer to \ref StmConfig structure
 * @param client: pointer to \ref netconn structure (represents endpoint client)
 * @retval ERR_OK if there are no errors
 */
err_t sendConfiguration(StmConfig* config, struct netconn* client,
		char* requestParameters) {
	char configContent[256];
	stmConfigToString(config, configContent);
	return sendData(client, requestParameters, configContent);
}

err_t sendHttpOk(struct netconn* client, char* requestParameters) {
	char response[256];
	sprintf(response, httpOkHeaderPattern, 0, requestParameters, "");
	return sendString(client, response);
}

err_t sendString(struct netconn* client, const char* array) {
	return netconn_write(client, array, strlen(array), NETCONN_NOCOPY);
}

/**
 * @brief Check if the request includes '/config' text
 * @param buf: pointer to \ref netbuf structure
 * @retval 1 if request includes '/config'
 */
uint8_t isConfigRequest(struct netbuf* buf) {
	void* data;
	uint16_t length;
	netbuf_data(buf, &data, &length);
	char* fullMsg = (char*) data;

	if (strstr(fullMsg, "/config") != NULL)
		return 1;
	return 0;
}
