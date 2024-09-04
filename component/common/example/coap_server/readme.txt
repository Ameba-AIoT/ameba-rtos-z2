COAP SERVER EXAMPLE

Description:
~~~~~~~~~~~
	This example demonstrates how to use libcoap C library to build a CoAP server.


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

	4) Enable CONFIG_EXAMPLE_COAP_CLIENT in platform_opts.h
	[platform_opts.h]
	#define CONFIG_EXAMPLE_COAP                 0
	#define CONFIG_EXAMPLE_COAP_SERVER          1


Execution:
~~~~~~~~~~
	Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
	A CoAP server example thread will be started automatically when booting.
	Can test with example_coap_client to connect to CoAP server, need to modify SERVER_HOST, SERVER_PORT in example_coap_client.c based on the CoAP server.
		[example_coap_client.c] SERVER_HOST format:
		char SERVER_HOST[30] = "coap://192.168.0.1";

	Note:
	Company Firewall may block CoAP message. Please try with public network.


Supported List
~~~~~~~~~~~~~~
[Supported List]
	Supported IC : 
		Ameba-z2