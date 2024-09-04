##################################################################################
#                                                                                #
#                             Example_coap                                       #
#                                                                                #
##################################################################################

Description
~~~~~~~~~~~
	This example demonstrates how to use mbed-CoAP C library to build and parse a CoAP message.

Setup Guide
~~~~~~~~~~~
	1) Make sure libcoap library (lib_coap.a/lib_coap_ns.a) is not included in porject.

	2) Make sure CoAP library is included to build in porject.
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

	3) Enable CONFIG_EXAMPLE_COAP in platform_opts.h
	/* for CoAP example*/
	#define CONFIG_EXAMPLE_COAP 1

	4) Modify SERVER_HOST, SERVER_PORT in example_coap.c based on your CoAP server.


Result description
~~~~~~~~~~~~~~~~~~
	Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
	A CoAP client example thread will be started automatically when booting.

	In the example, a confirmable GET request is send to test server to retrieve the resource under path. The expected return is an ACK message with payload containing the Greenwich Mean Time (GMT).

	Note: Company Firewall may block CoAP message. Please try with public network.


Supported List
~~~~~~~~~~~~~~
[Supported List]
	Supported IC :
		Ameba-1,
		Ameba-pro,
		Ameba-z2
	Not Supported IC:
		Ameba-z

