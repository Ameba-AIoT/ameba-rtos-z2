#include "platform_opts.h"

#if defined(CONFIG_EXAMPLE_OTA_HTTP) && CONFIG_EXAMPLE_OTA_HTTP
#include <ota_8710c.h>
#include <osdep_service.h>
#include "wifi_conf.h"

#define PORT	8082  //80
#define HOST	"192.168.1.53"  //"m-apps.oss-cn-shenzhen.aliyuncs.com"
#define RESOURCE ""     //"051103061600.bin"


#ifdef HTTP_OTA_UPDATE
void http_update_ota_task(void *param)
{

#if defined(configENABLE_TRUSTZONE) && (configENABLE_TRUSTZONE == 1)
	rtw_create_secure_context(configMINIMAL_SECURE_STACK_SIZE);
#endif
	int ret = -1;

	while (wifi_is_ready_to_transceive(RTW_STA_INTERFACE) != RTW_SUCCESS) {
		printf("Wait Wi-Fi to be connected.\r\n");
		vTaskDelay(5000 / portTICK_PERIOD_MS);
	}

	ret = http_update_ota(HOST, PORT, RESOURCE);

	printf("\n\r[%s] Update task exit", __FUNCTION__);
	if (!ret) {
		printf("\n\r[%s] Ready to reboot", __FUNCTION__);
		ota_platform_reset();
	}
	vTaskDelete(NULL);
}


void example_ota_http(void)
{
	if (xTaskCreate(http_update_ota_task, (char const *)"http_update_ota_task", 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
		printf("\n\r[%s] Create update task failed", __FUNCTION__);
	}
}
#endif //#ifdef HTTP_OTA_UPDATE

#endif //#if defined(CONFIG_EXAMPLE_OTA_HTTP) && CONFIG_EXAMPLE_OTA_HTTP