OTA HTTP UPDATING EXAMPLE

Description:
Download firmware from http download server(A local HTTP server is in tools\DownloadServer)

Configuration:
1. Modify PORT, HOST and RESOURCE based on your HTTP download server.
[example_ota_http.c]
	eg: SERVER: http://m-apps.oss-cn-shenzhen.aliyuncs.com/051103061600.bin
	set:
		#define PORT	80
		#define HOST	"m-apps.oss-cn-shenzhen.aliyuncs.com"
		#define RESOURCE "051103061600.bin"
	For local HTTP server download, set it with IP and firmware_xx.bin
	set:
		#define PORT	80
		#define HOST	"192.168.1.100"
		#define RESOURCE	"firmware_xx.bin"

2. Enable example.
[platform_opts.h]
	#define CONFIG_EXAMPLE_OTA_HTTP    1
[ota_8710c.h]
	#define HTTP_OTA_UPDATE

Execution:
Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
A http download example thread will be started automatically when booting.
A local HTTP server can be used in tools\DownloadServer.

[Supported List]
	Supported :
	    Ameba-1, Ameba-pro, Ameba-z2
	Source code not in project:
	    Ameba-z