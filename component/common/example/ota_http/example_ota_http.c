#if defined(CONFIG_PLATFORM_8711B)
#include "rtl8710b_ota.h"
#include <FreeRTOS.h>
#include <task.h>
#elif defined(CONFIG_PLATFORM_8195A)
#include <ota_8195a.h>
#elif defined(CONFIG_PLATFORM_8195BHP)
#include <ota_8195b.h>
#elif defined(CONFIG_PLATFORM_8710C)
#include <ota_8710c.h>
#endif
#include <osdep_service.h>

#define PORT	8082
#define HOST	"192.168.1.53"  //"m-apps.oss-cn-shenzhen.aliyuncs.com"
#define RESOURCE ""     //"051103061600.bin"


#ifdef HTTP_OTA_UPDATE
void http_update_ota_task(void *param){

#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtw_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
#endif
	
	printf("\n\r\n\r\n\r\n\r<<<<<<Waiting for 1 minute to connect Wi-Fi>>>>>>>\n\r\n\r\n\r\n\r");
	vTaskDelay(60*1000);
	int ret = -1;
	
	ret = http_update_ota(HOST, PORT, RESOURCE);

	printf("\n\r[%s] Update task exit", __FUNCTION__);
	if(!ret){
		printf("\n\r[%s] Ready to reboot", __FUNCTION__);	
		ota_platform_reset();
	}
	vTaskDelete(NULL);	
}


void example_ota_http(void){
		if(xTaskCreate(http_update_ota_task, (char const *)"http_update_ota_task", 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS){
		printf("\n\r[%s] Create update task failed", __FUNCTION__);
	}
}
#endif

