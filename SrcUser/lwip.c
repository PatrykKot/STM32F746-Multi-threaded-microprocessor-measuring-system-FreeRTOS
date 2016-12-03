/**
 ******************************************************************************
 * File Name          : LWIP.c
 * Description        : This file provides initialization code for LWIP
 *                      middleWare.
 ******************************************************************************
 * COPYRIGHT(c) 2016 STMicroelectronics
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *   1. Redistributions of source code must retain the above copyright notice,
 *      this list of conditions and the following disclaimer.
 *   2. Redistributions in binary form must reproduce the above copyright notice,
 *      this list of conditions and the following disclaimer in the documentation
 *      and/or other materials provided with the distribution.
 *   3. Neither the name of STMicroelectronics nor the names of its contributors
 *      may be used to endorse or promote products derived from this software
 *      without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "lwip.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "main.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* ETH Variables initialization ----------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* Variables Initialization */
struct netif ethernetInterfaceHandler;
ip4_addr_t ipaddr;
ip4_addr_t netmask;
ip4_addr_t gw;

/* USER CODE BEGIN 2 */

/* USER CODE END 2 */

/**
 * LwIP initialization function
 */
void MX_LWIP_Init(void) {
	/* Initilialize the LwIP stack with RTOS */
	tcpip_init( NULL, NULL);

	/* IP addresses initialization with DHCP (IPv4) */
	IP4_ADDR(&ipaddr, IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3);
	IP4_ADDR(&netmask, NETMASK_ADDR0, NETMASK_ADDR1, NETMASK_ADDR2,
			NETMASK_ADDR3);
	IP4_ADDR(&gw, GW_ADDR0, GW_ADDR1, GW_ADDR2, GW_ADDR3);

	/* add the network interface (IPv4/IPv6) with RTOS */
	netif_add(&ethernetInterfaceHandler, &ipaddr, &netmask, &gw, NULL, &ethernetif_init,
			&tcpip_input);

	/* Registers the default network interface */
	netif_set_default(&ethernetInterfaceHandler);

	//if (netif_is_link_up(&gnetif)) {
		/* When the netif is fully configured this function must be called */
		netif_set_up(&ethernetInterfaceHandler);
	//} else {
		/* When the netif link is down this function must be called */
	//	netif_set_down(&gnetif);
	//}
	/* USER CODE BEGIN 3 */

	/* USER CODE END 3 */
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
