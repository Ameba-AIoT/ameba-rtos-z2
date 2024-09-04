HIGH-LOAD USE CASE MEMORY USAGE EXAMPLE

Description:
Example to monitor the memory usage for high-load use case.
Includes:
	- 3 TLS sessions
	- 6 UDP sessions including 5 UDP servers on different ports and 1 UDP client

Configuration:
[platform_opts.h]
	#define CONFIG_EXAMPLE_WLAN_FAST_CONNECT	0
	#define CONFIG_EXAMPLE_HIGH_LOAD_MEMORY_USE	1
[lwipopts.h]
	#define MEMP_NUM_NETCONN		20
	#define MEMP_NUM_UDP_PCB		MEMP_NUM_NETCONN
[platform_stdlib_rtl8710c.h]
	#define BUFFERED_PRINTF         1
[example_high_load_memory_use.c]
	1. Set configuration of the AP to connect to.
		char *ssid = "Test_ap"; //change to your internet ssid
		char *password = "12345678"; //change to your internet password
	2. Set UDP server host.
		char* ip = "192.168.1.101";  //change to your UDP server ip addr
		int port = 6001;  //change to your UDP server port
	3. Set SSL server host.
	If use local apache server:
		ssl_client_thread("192.168.1.101"); //change to your local SSL server ip addr
		ssl_client_thread("192.168.1.101"); //change to your local SSL server ip addr
		ssl_client_thread("192.168.1.101"); //change to your local SSL server ip addr
	If not use local apache server:
		ssl_client_thread("192.168.1.101"); //change to "www.google.com"
		ssl_client_thread("192.168.1.101"); //change to "www.google.com"
		ssl_client_thread("192.168.1.101"); //change to "www.google.com"
		[lwip/opt.h]
		#define DNS_TABLE_SIZE                  3
		[config_rsa.h]
		#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
		#define MBEDTLS_ECDH_C
		#define MBEDTLS_ECP_C
		#define MBEDTLS_SSL_MAX_CONTENT_LEN                8192

Execution:
The example thread will be started automatically when booting.
You may need to change the IP/Port used in this example to make it work properly.
To verify the TLS sessions, you can setup an apache server for yourself and make sure the KeepAliveTimeout is larger than 5s.
To verify UDP server session, you can use iperf with following command to send UDP data:
	$ iperf.exe -c 192.168.1.123 -u -p 6001 -t 30 -i 1
To verify UDP client session, you can use iperf with following command to start UDP server:
	$ iperf.exe -s -p 6001 -u -i 1

Note:
Example work flow:
	Start heap monitor thread
	-> Enable Wi-Fi with STA mode
	-> Connect to AP by WPA2-AES
	-> Start 3 TLS sessions
	-> Start 6 UDP sessions

If you want to use ECDHE as TLS cipher suite, you can modify:
[config_rsa.h]
	#define MBEDTLS_KEY_EXCHANGE_ECDHE_RSA_ENABLED
	#define MBEDTLS_ECDH_C
	#define MBEDTLS_ECP_C

[Supported List]
	Supported :
	    Ameba-1, Ameba-z, Ameba-pro, Ameba-z2