/******************************************************************************
 *
 * Copyright(c) 2007 - 2015 Realtek Corporation. All rights reserved.
 *
 *
 ******************************************************************************/
#include <platform_opts.h>

#if defined(CONFIG_PLATFORM_8710C)
#include <platform_opts_bt.h>
#endif
#include "main.h"
#include "FreeRTOSConfig.h"

#if ATCMD_VER == ATVER_2
#include "flash_api.h"
#include "device_lock.h"
#endif

#if defined(CONFIG_EXAMPLE_MDNS) && CONFIG_EXAMPLE_MDNS
#include <mdns/example_mdns.h>
#endif

#if defined(CONFIG_EXAMPLE_MCAST) && CONFIG_EXAMPLE_MCAST
#include <mcast/example_mcast.h>
#endif

#if defined(CONFIG_EXAMPLE_XML) && CONFIG_EXAMPLE_XML
#include <xml/example_xml.h>
#endif

#if defined(CONFIG_EXAMPLE_UART_ATCMD) && CONFIG_EXAMPLE_UART_ATCMD
#include <uart_atcmd/example_uart_atcmd.h>
#include <freertos_pmu.h>
#endif

#if defined(CONFIG_EXAMPLE_SOCKET_SELECT) && CONFIG_EXAMPLE_SOCKET_SELECT
#include <socket_select/example_socket_select.h>
#endif

#if defined(CONFIG_EXAMPLE_NONBLOCK_CONNECT) && CONFIG_EXAMPLE_NONBLOCK_CONNECT
#include <nonblock_connect/example_nonblock_connect.h>
#endif

#if defined(CONFIG_EXAMPLE_SOCKET_TCP_TRX) && CONFIG_EXAMPLE_SOCKET_TCP_TRX
#include <socket_tcp_trx/example_socket_tcp_trx.h>
#endif

#if defined(CONFIG_EXAMPLE_SMTP) && CONFIG_EXAMPLE_SMTP
#include <smtp/example_smtp.h>
#endif

#if defined(CONFIG_EXAMPLE_SSL_DOWNLOAD) && CONFIG_EXAMPLE_SSL_DOWNLOAD
#include <ssl_download/example_ssl_download.h>
#endif

#if defined(CONFIG_EXAMPLE_HTTP_CLIENT) && CONFIG_EXAMPLE_HTTP_CLIENT
#include <http_client/example_http_client.h>
#endif

#if defined(CONFIG_EXAMPLE_HTTP_DOWNLOAD) && CONFIG_EXAMPLE_HTTP_DOWNLOAD
#include <http_download/example_http_download.h>
#endif

#if defined(CONFIG_EXAMPLE_HTTPC) && CONFIG_EXAMPLE_HTTPC
#include <httpc/example_httpc.h>
#endif

#if defined(CONFIG_EXAMPLE_FFS) && CONFIG_EXAMPLE_FFS
//#include <amazon_ffs/example_ffs.h>
#endif

#if defined(CONFIG_EXAMPLE_HTTPD) && CONFIG_EXAMPLE_HTTPD
#include <httpd/example_httpd.h>
#endif

#if defined(CONFIG_EXAMPLE_HTTP2_CLIENT) && CONFIG_EXAMPLE_HTTP2_CLIENT
#include <http2_client/example_http2_client.h>
#endif

#if defined(CONFIG_EXAMPLE_TCP_KEEPALIVE) && CONFIG_EXAMPLE_TCP_KEEPALIVE
#include <tcp_keepalive/example_tcp_keepalive.h>
#endif

#if defined(CONFIG_EXAMPLE_SNTP_SHOWTIME) && CONFIG_EXAMPLE_SNTP_SHOWTIME
#include <sntp_showtime/example_sntp_showtime.h>
#endif

#if defined(CONFIG_EXAMPLE_PPPOE) && CONFIG_EXAMPLE_PPPOE
#include <pppoe/example_pppoe.h>
#endif

#if defined(CONFIG_EXAMPLE_AZURE_IOTHUB_TELEMETRY) && CONFIG_EXAMPLE_AZURE_IOTHUB_TELEMETRY
#include <azure_iothub_telemetry/example_azure_iothub_telemetry.h>
#endif

#if defined(CONFIG_EXAMPLE_AZURE_IOTHUB_X509) && CONFIG_EXAMPLE_AZURE_IOTHUB_X509
#include <azure_iothub_x509/example_azure_iothub_x509.h>
#endif

#if defined(CONFIG_EXAMPLE_AZURE) && CONFIG_EXAMPLE_AZURE
#include <azure/example_azure_iot_entry.h>
#endif

#if defined(CONFIG_EXAMPLE_GOOGLE_NEST) && CONFIG_EXAMPLE_GOOGLE_NEST
#include <googlenest/example_google.h>
#define FromDevice            1
#define ToDevice     		2
#define BOTH     		3
#define TYPE         "ToDevice"
#endif

#if defined(CONFIG_EXAMPLE_GOOGLE_CLOUD_IOT) && CONFIG_EXAMPLE_GOOGLE_CLOUD_IOT
#include <google_cloud_iot/example_google_cloud_iot.h>
#endif

#if defined(CONFIG_EXAMPLE_TRUST_ZONE) && (CONFIG_EXAMPLE_TRUST_ZONE)
#include <trust_zone/example_trust_zone.h>
#endif

#if defined(CONFIG_EXAMPLE_WLAN_FAST_CONNECT) && CONFIG_EXAMPLE_WLAN_FAST_CONNECT
#include <wlan_fast_connect/example_wlan_fast_connect.h>
#endif

#if defined(CONFIG_EXAMPLE_MQTT) && CONFIG_EXAMPLE_MQTT
#include <mqtt/example_mqtt.h>
#endif

#if defined(CONFIG_EXAMPLE_FATFS) && CONFIG_EXAMPLE_FATFS
#include <fatfs/example_fatfs.h>
#endif

#if defined(CONFIG_EXAMPLE_DCT) && CONFIG_EXAMPLE_DCT
#include <dct/example_dct.h>
#endif

#if defined(CONFIG_EXAMPLE_ARDUINO_WIFI) && CONFIG_EXAMPLE_ARDUINO_WIFI
#include <arduino_wifi/ard_spi.h>
#endif

#if defined(CONFIG_EXAMPLE_GET_BEACON_FRAME) && CONFIG_EXAMPLE_GET_BEACON_FRAME
#include <get_beacon_frame/example_get_beacon_frame.h>
#endif

#if defined(CONFIG_EXAMPLE_EAP) && CONFIG_EXAMPLE_EAP
#include <eap/example_eap.h>
#endif

#if defined(CONFIG_EXAMPLE_COAP) && CONFIG_EXAMPLE_COAP
#include <coap/example_coap.h>
#endif

#if defined(CONFIG_EXAMPLE_COAP_CLIENT) && CONFIG_EXAMPLE_COAP_CLIENT
#include <coap_client/example_coap_client.h>
#endif

#if defined(CONFIG_EXAMPLE_COAP_SERVER) && CONFIG_EXAMPLE_COAP_SERVER
#include <coap_server/example_coap_server.h>
#endif

#if defined(CONFIG_EXAMPLE_WEBSOCKET_CLIENT) && CONFIG_EXAMPLE_WEBSOCKET_CLIENT
#include <websocket_client/example_wsclient.h>
#endif

#if defined(CONFIG_EXAMPLE_WEBSOCKET_SERVER) && CONFIG_EXAMPLE_WEBSOCKET_SERVER
#include <websocket_server/example_ws_server.h>
#endif

#if defined(CONFIG_EXAMPLE_BCAST) && CONFIG_EXAMPLE_BCAST
#include <bcast/example_bcast.h>
#endif

#if defined(CONFIG_EXAMPLE_HIGH_LOAD_MEMORY_USE) && CONFIG_EXAMPLE_HIGH_LOAD_MEMORY_USE
#include <high_load_memory_use/example_high_load_memory_use.h>
#endif

#if defined(CONFIG_EXAMPLE_WIFI_MAC_MONITOR) && CONFIG_EXAMPLE_WIFI_MAC_MONITOR
#include <wifi_mac_monitor/example_wifi_mac_monitor.h>
#endif

#if defined(CONFIG_EXAMPLE_RARP) && CONFIG_EXAMPLE_RARP
#include <rarp/example_rarp.h>
#endif

#if (defined(CONFIG_EXAMPLE_UART_ATCMD) && CONFIG_EXAMPLE_UART_ATCMD)
#include <uart_atcmd/example_uart_atcmd.h>
#endif

#if defined(CONFIG_EXAMPLE_SSL_SERVER) && CONFIG_EXAMPLE_SSL_SERVER
#include <ssl_server/example_ssl_server.h>
#endif

#if defined(CONFIG_EXAMPLE_OTA_HTTP) && CONFIG_EXAMPLE_OTA_HTTP
#include <ota_http/example_ota_http.h>
#endif

#if defined(CONFIG_EXAMPLE_OTA_HTTPS) && CONFIG_EXAMPLE_OTA_HTTPS
#include <ota_https/example_ota_https.h>
#endif

#if defined(CONFIG_EXAMPLE_WIFI_ROAMING) && CONFIG_EXAMPLE_WIFI_ROAMING
#include <wifi_roaming/example_wifi_roaming.h>
#endif

#if defined(CONFIG_EXAMPLE_TICKLESS_WIFI_ROAMING) && CONFIG_EXAMPLE_TICKLESS_WIFI_ROAMING
#include <tickless_wifi_roaming/example_tickless_wifi_roaming.h>
#endif

#if defined(CONFIG_EXAMPLE_CJSON) && CONFIG_EXAMPLE_CJSON
#include <cJSON/example_cJSON.h>
#endif

#if defined(CONFIG_BT) && CONFIG_BT
#include <bt_example_entry.h>
#endif

#if defined(CONFIG_EXAMPLE_SECURE_BOOT) && CONFIG_EXAMPLE_SECURE_BOOT
#include <secure_boot/example_secure_boot.h>
#endif

#if defined(CONFIG_EXAMPLE_SECURE_STORAGE) && CONFIG_EXAMPLE_SECURE_STORAGE
#include <secure_storage/example_secure_storage.h>
#endif

#if defined(CONFIG_BT_MESH_PROVISIONER_RTK_DEMO) && CONFIG_BT_MESH_PROVISIONER_RTK_DEMO
#include "example_bt_mesh_provisioner_rtk_demo.h"
#endif

#if defined(CONFIG_BT_MESH_DEVICE_RTK_DEMO) && CONFIG_BT_MESH_DEVICE_RTK_DEMO
#include "example_bt_mesh_device_rtk_demo.h"
#endif

#if defined(CONFIG_EXAMPLE_IPV6) && CONFIG_EXAMPLE_IPV6
#include <ipv6/example_ipv6.h>
#endif

#if defined(CONFIG_EXAMPLE_AMAZON_FREERTOS) && (CONFIG_EXAMPLE_AMAZON_FREERTOS == 1)
//#include <amazon_freertos/example_amazon_freertos.h>
#endif

#if defined(CONFIG_EXAMPLE_MBEDTLS_ECDHE) && (CONFIG_EXAMPLE_MBEDTLS_ECDHE == 1)
#include <mbedtls_ecdhe/example_mbedtls_ecdhe.h>
#endif

#if defined(CONFIG_EXAMPLE_MATTER) && (CONFIG_EXAMPLE_MATTER == 1)
#include <matter_example_entry.h>
#endif

/*
	Preprocessor of example
*/
void pre_example_entry(void)
{

#if defined(CONFIG_EXAMPLE_CM_BACKTRACE) && CONFIG_EXAMPLE_CM_BACKTRACE
	cm_backtrace_init("application", "HW v1.0", "SW v1.0");
#endif

#if ATCMD_VER == ATVER_2
	flash_t flash;
	struct wlan_fast_reconnect read_data = {0};
	device_mutex_lock(RT_DEV_LOCK_FLASH);
	flash_stream_read(&flash, FAST_RECONNECT_DATA, sizeof(struct wlan_fast_reconnect), (u8 *) &read_data);
	device_mutex_unlock(RT_DEV_LOCK_FLASH);
#endif

#if defined(CONFIG_EXAMPLE_WLAN_FAST_CONNECT) && CONFIG_EXAMPLE_WLAN_FAST_CONNECT
#if ATCMD_VER == ATVER_2
	if (read_data.enable == 1)
#endif
	{
		example_wlan_fast_connect();
	}
#endif
#if defined(CONFIG_EXAMPLE_UART_ADAPTER) && CONFIG_EXAMPLE_UART_ADAPTER
	example_uart_adapter_init();
#endif
#if defined(CONFIG_EXAMPLE_ARDUINO_WIFI) && CONFIG_EXAMPLE_ARDUINO_WIFI
	/*To ensure the application has enough heap size, please goto FreeRTOSConfig.h to change configTOTAL_HEAP_SIZE at least to 80*1024 */
	example_arduino_wifi_init();
#endif

#if defined(configGENERATE_RUN_TIME_STATS) && (configGENERATE_RUN_TIME_STATS == 1)
	//pmu_enable_wakelock_stats(1);
#endif

#if defined(CONFIG_PLATFORM_8710C) && defined(CONFIG_FTL_ENABLED)
	app_ftl_init();
#endif

#if defined(CONFIG_EXAMPLE_TICKLESS_WIFI_ROAMING) && CONFIG_EXAMPLE_TICKLESS_WIFI_ROAMING
	example_tickless_wifi_roaming_init();
#endif
}

#if ((defined CONFIG_BT_MESH_PROVISIONER_RTK_DEMO && CONFIG_BT_MESH_PROVISIONER_RTK_DEMO) || (defined CONFIG_BT_MESH_DEVICE_RTK_DEMO && CONFIG_BT_MESH_DEVICE_RTK_DEMO))
extern void example_bt_mesh(void);
#endif

/*
  	All of the examples are disabled by default for code size consideration
   	The configuration is enabled in platform_opts.h
*/
void example_entry(void)
{
#if ((defined(CONFIG_EXAMPLE_MDNS) && CONFIG_EXAMPLE_MDNS) && !CONFIG_EXAMPLE_UART_ADAPTER)
	example_mdns();
#endif

#if defined(CONFIG_EXAMPLE_MCAST) && CONFIG_EXAMPLE_MCAST
	example_mcast();
#endif

#if defined(CONFIG_EXAMPLE_XML) && CONFIG_EXAMPLE_XML
	example_xml();
#endif

#if defined(CONFIG_EXAMPLE_SOCKET_SELECT) && CONFIG_EXAMPLE_SOCKET_SELECT
	example_socket_select();
#endif

#if defined(CONFIG_EXAMPLE_NONBLOCK_CONNECT) && CONFIG_EXAMPLE_NONBLOCK_CONNECT
	example_nonblock_connect();
#endif

#if defined(CONFIG_EXAMPLE_SOCKET_TCP_TRX)
#if (CONFIG_EXAMPLE_SOCKET_TCP_TRX == 1)
	example_socket_tcp_trx_1();
#elif (CONFIG_EXAMPLE_SOCKET_TCP_TRX == 2)
	example_socket_tcp_trx_2();
#endif
#endif

#if defined(CONFIG_EXAMPLE_SMTP) && CONFIG_EXAMPLE_SMTP
	example_smtp();
#endif

#if defined(CONFIG_EXAMPLE_SSL_DOWNLOAD) && CONFIG_EXAMPLE_SSL_DOWNLOAD
	example_ssl_download();
#endif

#if defined(CONFIG_EXAMPLE_HTTP_DOWNLOAD) && CONFIG_EXAMPLE_HTTP_DOWNLOAD
	example_http_download();
#endif

#if defined(CONFIG_EXAMPLE_HTTPC) && CONFIG_EXAMPLE_HTTPC
	example_httpc();
#endif

#if defined(CONFIG_EXAMPLE_FFS) && CONFIG_EXAMPLE_FFS
	//example_amazon_ffs();
#endif

#if defined(CONFIG_EXAMPLE_HTTPD) && CONFIG_EXAMPLE_HTTPD
	example_httpd();
#endif

#if defined(CONFIG_EXAMPLE_HTTP2_CLIENT) && CONFIG_EXAMPLE_HTTP2_CLIENT
	example_http2_client();
#endif

#if defined(CONFIG_EXAMPLE_TCP_KEEPALIVE) && CONFIG_EXAMPLE_TCP_KEEPALIVE
	example_tcp_keepalive();
#endif

#if defined(CONFIG_EXAMPLE_SNTP_SHOWTIME) && CONFIG_EXAMPLE_SNTP_SHOWTIME
	example_sntp_showtime();
#endif

#if defined(CONFIG_EXAMPLE_PPPOE) && CONFIG_EXAMPLE_PPPOE
	example_pppoe();
#endif

#if defined(CONFIG_EXAMPLE_GOOGLE_NEST) && CONFIG_EXAMPLE_GOOGLE_NEST
	example_google(TYPE);
#endif

#if defined(CONFIG_EXAMPLE_GOOGLE_CLOUD_IOT) && CONFIG_EXAMPLE_GOOGLE_CLOUD_IOT
	example_google_cloud_iot();
#endif

#if defined(CONFIG_EXAMPLE_MQTT) && CONFIG_EXAMPLE_MQTT
	example_mqtt();
#endif


#if defined(CONFIG_JOYLINK) && CONFIG_JOYLINK
	example_joylink();
#endif
#if defined(CONFIG_EXAMPLE_WIFI_MAC_MONITOR) && CONFIG_EXAMPLE_WIFI_MAC_MONITOR
	example_wifi_mac_monitor();
#endif

#if defined(CONFIG_EXAMPLE_HTTP_CLIENT) && CONFIG_EXAMPLE_HTTP_CLIENT
	example_http_client();
#endif

#if defined(CONFIG_EXAMPLE_GET_BEACON_FRAME) && CONFIG_EXAMPLE_GET_BEACON_FRAME
	example_get_beacon_frame();
#endif

#if defined(CONFIG_EXAMPLE_FATFS) && CONFIG_EXAMPLE_FATFS
	example_fatfs();
#endif

#if defined(CONFIG_EXAMPLE_DCT) && CONFIG_EXAMPLE_DCT
	example_dct();
#endif

#if (defined(CONFIG_EXAMPLE_UART_ATCMD) && CONFIG_EXAMPLE_UART_ATCMD)
	example_uart_atcmd();
#endif

#if defined(CONFIG_EXAMPLE_SPI_ATCMD) && CONFIG_EXAMPLE_SPI_ATCMD
	example_spi_atcmd();
#endif

// supported eap methods: tls, peap, ttls
// make sure the corresponding config has been turned on before you use it
// e.g. set CONFIG_ENABLE_TLS to 1 if you want to use TLS eap method
#if defined(CONFIG_EXAMPLE_EAP) && CONFIG_EXAMPLE_EAP
	example_eap("tls");
	//example_eap("peap");
	//example_eap("ttls");
#endif

#if defined(CONFIG_EXAMPLE_COAP) && CONFIG_EXAMPLE_COAP
	example_coap();
#endif

#if defined(CONFIG_EXAMPLE_COAP_CLIENT) && CONFIG_EXAMPLE_COAP_CLIENT
	example_coap_client();
#endif

#if defined(CONFIG_EXAMPLE_COAP_SERVER) && CONFIG_EXAMPLE_COAP_SERVER
	example_coap_server();
#endif

#if defined(CONFIG_EXAMPLE_WEBSOCKET_CLIENT) && CONFIG_EXAMPLE_WEBSOCKET_CLIENT
	example_wsclient();
#endif

#if defined(CONFIG_EXAMPLE_WEBSOCKET_SERVER) && CONFIG_EXAMPLE_WEBSOCKET_SERVER
	example_wsserver();
#endif

#if defined(CONFIG_EXAMPLE_BCAST) && CONFIG_EXAMPLE_BCAST
	example_bcast();
#endif

#if defined(CONFIG_EXAMPLE_HIGH_LOAD_MEMORY_USE) && CONFIG_EXAMPLE_HIGH_LOAD_MEMORY_USE
	example_high_load_memory_use();
#endif

#if defined(CONFIG_EXAMPLE_RARP) && CONFIG_EXAMPLE_RARP
	example_rarp();
#endif

#if defined(CONFIG_EXAMPLE_SSL_SERVER) && CONFIG_EXAMPLE_SSL_SERVER
	example_ssl_server();
#endif

#if defined(CONFIG_EXAMPLE_OTA_HTTP) && CONFIG_EXAMPLE_OTA_HTTP
	example_ota_http();
#endif

#if defined(CONFIG_EXAMPLE_OTA_HTTPS) && CONFIG_EXAMPLE_OTA_HTTPS
	example_ota_https();
#endif

#if defined(CONFIG_EXAMPLE_AZURE_IOTHUB_TELEMETRY) && CONFIG_EXAMPLE_AZURE_IOTHUB_TELEMETRY
	example_azure_iothub_telemetry();
#endif

#if defined(CONFIG_EXAMPLE_AZURE_IOTHUB_X509) && CONFIG_EXAMPLE_AZURE_IOTHUB_X509
	example_azure_iothub_x509();
#endif

#if defined(CONFIG_EXAMPLE_AZURE) && CONFIG_EXAMPLE_AZURE
	example_azure();
#endif

#if defined(CONFIG_EXAMPLE_WIFI_ROAMING) && CONFIG_EXAMPLE_WIFI_ROAMING
	example_wifi_roaming();
#endif

#if defined(CONFIG_EXAMPLE_TICKLESS_WIFI_ROAMING) && CONFIG_EXAMPLE_TICKLESS_WIFI_ROAMING
	example_tickless_wifi_roaming();
#endif

#if defined(CONFIG_EXAMPLE_CJSON) && CONFIG_EXAMPLE_CJSON
	example_cJSON();
#endif

#if defined(CONFIG_EXAMPLE_SECURE_BOOT) && (CONFIG_EXAMPLE_SECURE_BOOT == 1)
	example_secure_boot();
#endif

#if defined(CONFIG_EXAMPLE_SECURE_STORAGE) && (CONFIG_EXAMPLE_SECURE_STORAGE == 1)
	example_secure_storage();
#endif

#if defined(CONFIG_EXAMPLE_TRUST_ZONE) && (CONFIG_EXAMPLE_TRUST_ZONE == 1)
	example_trust_zone();
#endif

#if defined(CONFIG_EXAMPLE_IPV6) && CONFIG_EXAMPLE_IPV6
	example_ipv6();
#endif

#if defined(CONFIG_EXAMPLE_AMAZON_FREERTOS) && (CONFIG_EXAMPLE_AMAZON_FREERTOS == 1)
	//example_amazon_freertos();
#endif

#if ((defined CONFIG_BT_MESH_PROVISIONER_RTK_DEMO && CONFIG_BT_MESH_PROVISIONER_RTK_DEMO) || (defined CONFIG_BT_MESH_DEVICE_RTK_DEMO && CONFIG_BT_MESH_DEVICE_RTK_DEMO))
	example_bt_mesh();
#endif

#if defined(CONFIG_EXAMPLE_AMAZON_AFQP_TESTS) && CONFIG_EXAMPLE_AMAZON_AFQP_TESTS
	//example_amazon_afqp_tests();
#endif

#if defined(CONFIG_EXAMPLE_MATTER) && (CONFIG_EXAMPLE_MATTER == 1)
	matter_example_entry();
#endif

#if defined(API_TEST_MODE) && defined(API_TEST_MODE)
#include <api_test_entry.h>
	api_test_entry();
#endif

#if defined(CONFIG_EXAMPLE_MBEDTLS_ECDHE) && (CONFIG_EXAMPLE_MBEDTLS_ECDHE == 1)
	example_mbedtls_ecdhe();
#endif
}
