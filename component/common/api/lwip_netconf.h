/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
******************************************************************************/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __NETCONF_H
#define __NETCONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "tcpip.h"
#include "lwip/init.h" //for lwip version control
/* Includes ------------------------------------------------------------------*/
#include <platform/platform_stdlib.h>
#include "platform_opts.h"
#include "autoconf.h"

// macros
/* Give default value if not defined */
#ifndef NET_IF_NUM
#ifdef CONFIG_CONCURRENT_MODE
#define NET_IF_NUM ((CONFIG_WLAN) + 1)
#else
#define NET_IF_NUM CONFIG_WLAN
#endif  // end of CONFIG_CONCURRENT_MODE
#endif  // end of NET_IF_NUM

/* Private typedef -----------------------------------------------------------*/
typedef enum {
	DHCP_START = 0,
	DHCP_WAIT_ADDRESS,
	DHCP_ADDRESS_ASSIGNED,
	DHCP_RELEASE_IP,
	DHCP_STOP,
	DHCP_TIMEOUT
} DHCP_State_TypeDef;

#if LWIP_IPV6
#if LWIP_IPV6_DHCP6 && (LWIP_VERSION_MAJOR >= 2) && (LWIP_VERSION_MINOR >= 1)
typedef enum {
	DHCP6_START = 0,
	DHCP6_WAIT_ADDRESS,
	DHCP6_ADDRESS_ASSIGNED,
	DHCP6_RELEASE_IP,
	DHCP6_STOP,
	DHCP6_TIMEOUT
} DHCP6_State_TypeDef;
#endif
#endif

/* Extern functions ------------------------------------------------------------*/
void wifi_rx_beacon_hdl(char *buf, int buf_len, int flags, void *userdata);


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void LwIP_Init(void);
void LwIP_ReleaseIP(uint8_t idx);
uint8_t LwIP_DHCP(uint8_t idx, uint8_t dhcp_state);
uint8_t *LwIP_GetMAC(struct netif *pnetif);
uint8_t *LwIP_GetIP(struct netif *pnetif);
uint8_t *LwIP_GetGW(struct netif *pnetif);
uint8_t *LwIP_GetMASK(struct netif *pnetif);
uint8_t *LwIP_GetBC(struct netif *pnetif);
#if LWIP_DNS
void LwIP_GetDNS(struct ip_addr *dns);
void LwIP_SetDNS(struct ip_addr *dns);
#endif
void LwIP_UseStaticIP(struct netif *pnetif);
#if LWIP_AUTOIP
void LwIP_AUTOIP(struct netif *pnetif);
#endif
#if LWIP_IPV6
uint8_t *LwIP_GetIPv6_linklocal(struct netif *pnetif);
#if LWIP_IPV6_DHCP6 && (LWIP_VERSION_MAJOR >= 2) && (LWIP_VERSION_MINOR >= 1)
uint8_t LwIP_DHCP6(uint8_t idx, uint8_t dhcp_state);
uint8_t *LwIP_GetIPv6_global(struct netif *pnetif);
#endif
#endif
uint32_t LWIP_Get_Dynamic_Sleep_Interval(void);
extern struct netif xnetif[];
#ifdef __cplusplus
}
#endif

#endif /* __NETCONF_H */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
