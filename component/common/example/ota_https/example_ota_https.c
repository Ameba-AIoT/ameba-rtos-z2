#include "platform_opts.h"

#if defined(CONFIG_EXAMPLE_OTA_HTTPS) && CONFIG_EXAMPLE_OTA_HTTPS
#include <FreeRTOS.h>
#include <task.h>
#include <ota_8710c.h>
#include <osdep_service.h>
#include "wifi_conf.h"

#define PORT		443
#define HOST		"192.168.1.100"   //"m-apps.oss-cn-shenzhen.aliyuncs.com"
#define RESOURCE	"firmware_is.bin" //"051103061600.bin"
#define STACKSIZE	2048

#ifdef HTTPS_OTA_UPDATE
void https_update_ota_task(void *param)
{
#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtw_create_secure_context(STACKSIZE * 2);
#endif
	int ret = -1;

	while (wifi_is_ready_to_transceive(RTW_STA_INTERFACE) != RTW_SUCCESS) {
		printf("Wait Wi-Fi to be connected.\r\n");
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}

	ret = https_update_ota(HOST, PORT, RESOURCE);

	printf("\n\r[%s] Update task exit", __FUNCTION__);
	if (!ret) {
		printf("\n\r[%s] Ready to reboot", __FUNCTION__);
		ota_platform_reset();
	}
	vTaskDelete(NULL);
}


void example_ota_https(void)
{
	if (xTaskCreate(https_update_ota_task, (char const *)"https_update_ota_task", STACKSIZE, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\n\r[%s] Create update task failed", __FUNCTION__);
	}
}
#endif //#ifdef HTTPS_OTA_UPDATE
#endif //#if defined(CONFIG_EXAMPLE_OTA_HTTPS) && CONFIG_EXAMPLE_OTA_HTTPS