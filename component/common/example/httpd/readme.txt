##################################################################################
#                                                                                #
#                             example_httpd                                      #
#                                                                                #
##################################################################################

Date: 2018-09-10

Table of Contents
~~~~~~~~~~~~~~~~~
 - Description
 - Setup Guide
 - Parameter Setting and Configuration
 - Result description
 - Supported List

 
Description
~~~~~~~~~~~
		This example is based on HTTPD API, an HTTP/HTTPS server example with a simple homepage and GET/POST method test pages are provided.
		In this example only IPv4 are used to download files from Web server via http

Setup Guide
~~~~~~~~~~~
		In platform_opts.h, please set #define CONFIG_EXAMPLE_HTTPD  1
		In lwipopts.h, please set #define SO_REUSE  1

Parameter Setting and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
About SSL configuration:
Ameba-1 and Ameba-Z support both POLARSSL and MBEDTLS library.
AmebaPro and AmebaZ2 only support MBEDTLS.

[config_rsa.h]:
	SSL Configuration for HTTPS:
		#define MBEDTLS_CERTS_C
		#define MBEDTLS_SSL_SRV_C

[component\common\application\matter\common\mbedtls\mbedtls_config.h]
	For Matter Ameba-Z2:
		#define MBEDTLS_CERTS_C
		#define MBEDTLS_SSL_SRV_C

		Can make automatical Wi-Fi connection when booting by using wlan fast connect example.

Result description
~~~~~~~~~~~~~~~~~~
		This example is a httpd example thread is started automatically when booting.
		Both HTTP and HTTPS are supported by this exmaple, and can be changed by modifying USE_HTTPS.
		Can test with a Web browser connecting to the homepage of httpd server.
		Should link PolarSSL bignum.c to SRAM to speed up SSL handshake if starting HTTPS server.

Supported List
~~~~~~~~~~~~~~
[Supported List]
		Supported IC :
			Ameba-pro, Ameba-Z, Ameba-1, Ameba-Z2

Trouble Shooting
~~~~~~~~~~~~~~
		ERROR: [HTTPD] ERROR: mbedtls_ssl_handshake -30592, browser shows "Your connection is not private NET::ERR_CERT_AUTHORITY_INVALID"
		Solution: click Advanced and Proceed to 172.25.23.138 (unsafe).
				  As the certs are self-signed by mbedtls only for demo purpose, it may expired and browser could not find it on CA authorities
				  but it wouldn't effect the functionality