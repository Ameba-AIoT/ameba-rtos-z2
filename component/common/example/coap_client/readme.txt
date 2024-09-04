COAP CLIENT EXAMPLE

Description:
~~~~~~~~~~~
	This example demonstrates how to use libcoap C library to build a CoAP client.


Setup Guide
~~~~~~~~~~~
	1) Exclude CoAP library from build in project. (Note: after exclude lib from build, need to make clean before rebuild)
	/component/common/network/coap
	.
	|-- include
	| |-- ns_list.h
	| |-- ns_types.h
	| |-- sn_coap_ameba_port.h
	| |-- sn_coap_header.h
	| |-- sn_coap_header_internal.h
	| |-- sn_coap_protocol.h
	| |-- sn_coap_protocol_internal.h
	| `-- sn_config.h
	|-- sn_coap_ameba_port.c
	|-- sn_coap_builder.c
	|-- sn_coap_header_check.c
	|-- sn_coap_parser.c
	`-- sn_coap_protocol.c

	2) Include libcoap library (lib_coap.a/lib_coap_ns.a) to build in project.

	3) Config lwipopts.h
	[lwipopts.h]
	#define LWIP_TIMEVAL_PRIVATE            1
	#define SO_REUSE                        1
	#define MEMP_USE_CUSTOM_POOLS           1
	#define LWIP_IPV6                       1
	#ifndef xchar
	#define xchar(i)             ((i) < 10 ? '0' + (i) : 'A' + (i) - 10)
	#endif

	4) Enable CONFIG_EXAMPLE_COAP_CLIENT in platform_opts.h
	[platform_opts.h]
	#define CONFIG_EXAMPLE_COAP                 0
	#define CONFIG_EXAMPLE_COAP_CLIENT	    1

	5) Modify SERVER_HOST, SERVER_PORT in example_coap_client.c based on your CoAP server.


Execution:
~~~~~~~~~~
	Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
	A CoAP client example thread will be started automatically when booting.

	In default, the client will attempt to contact the server at coap://coap.me/hello with port 5683.
	The server will return a string with the word "world" to the client.

	Note:
	Company Firewall may block CoAP message. Please try with public network.


Supported List
~~~~~~~~~~~~~~
[Supported List]
	Supported IC : 
		Ameba-z2
