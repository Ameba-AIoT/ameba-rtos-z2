UART ATCMD EXAMPLE

Description:
Example for AT command through external UART connection

Configuration:

[platform_opts.h]
    #define CONFIG_EXAMPLE_UART_ATCMD            1

Connect to PC
    1, Connect Ground: connect to GND pin via USBtoTTL adapter
    2, Use UART0
        1), PA_13 as UART0_RX connect to TX of USBtoTTL adapter
        2), PA_14 as UART0_TX connect to RX of USBtoTTL adapter
        
Open Super terminal or Teraterm and set baud rate to 115200, 1 stopbit, no parity, no flow control.

Execution:
Please go through the AN0075 documentation for AT commands through external UART connection

if use ssl:
[config_rsa.h]
	#define MBEDTLS_BIGNUM_C
	#define MBEDTLS_CERTS_C
	#define MBEDTLS_SSL_TLS_C
	#define MBEDTLS_SSL_SRV_C
	#define MBEDTLS_SSL_CLI_C
	#define MBEDTLS_NET_C
	#define MBEDTLS_RSA_C
	#define MBEDTLS_X509_CRT_PARSE_C
[atcmd_lwip.h]
	#define ATCMD_SUPPORT_SSL	1
Steps to use:
	ATPW -> ATPN -> ATCK -> ATPC

[Supported List]
	Supported :
	    Ameba-1, Ameba-z, Ameba-pro, Ameba-z2