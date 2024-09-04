/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
******************************************************************************/
#include "osdep_service.h"
#include "main.h"
#if CONFIG_WLAN
#include "wifi_conf.h"
#include "wlan_intf.h"
#include "wifi_constants.h"
#endif
#include "lwip_netconf.h"
#include <platform/platform_stdlib.h>
#include "osdep_service.h"

#ifndef CONFIG_INIT_NET
#define CONFIG_INIT_NET             1
#endif

#define STACKSIZE                   (512 + 768)

_sema uart_rx_interrupt_sema = NULL;

void init_thread(void *param)
{
	/* To avoid gcc warnings */
	(void) param;
#if CONFIG_INIT_NET
#if CONFIG_LWIP_LAYER
	/* Initilaize the LwIP stack */
	LwIP_Init();
#endif
#endif
#if CONFIG_WIFI_IND_USE_THREAD
	wifi_manager_init();
#endif
#if CONFIG_WLAN
	wifi_on(RTW_MODE_STA);
#if defined(CONFIG_AUTO_RECONNECT) && CONFIG_AUTO_RECONNECT
	//setup reconnection flag
	wifi_set_autoreconnect(1);
#endif
	printf("\n\r%s(%d), Available heap 0x%x", __FUNCTION__, __LINE__, xPortGetFreeHeapSize());
#endif

	/* Kill init thread after all init tasks done */
	vTaskDelete(NULL);
}

void wlan_network()
{
#ifdef PLATFORM_OHOS
	if (xTaskCreate(init_thread, ((const char *)"init"), STACKSIZE, NULL, 10 - 3, NULL) != pdPASS)
#else
	if (xTaskCreate(init_thread, ((const char *)"init"), STACKSIZE, NULL, tskIDLE_PRIORITY + 3 + PRIORITIE_OFFSET, NULL) != pdPASS)
#endif
		printf("\n\r%s xTaskCreate(init_thread) failed", __FUNCTION__);
}
