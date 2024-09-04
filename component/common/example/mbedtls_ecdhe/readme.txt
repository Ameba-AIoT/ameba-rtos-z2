##################################################################################
#                                                                                #
#                             example_mbedtls_ecdhe                              #
#                                                                                #
##################################################################################

Date: 2021-09-08

Table of Contents
~~~~~~~~~~~~~~~~~
 - Description
 - Setup Guide
 - Parameter Setting and Configuration
 - Result description
 - Supported List
 
Description
~~~~~~~~~~~
	This example will show mbedtls ecdhe encryption.
	
Setup Guide
~~~~~~~~~~~
	[platform_opts.h]
		#define CONFIG_EXAMPLE_MBEDTLS_ECDHE		1
		#define CONFIG_EXAMPLE_WLAN_FAST_CONNECT 	0

Parameter Setting and Configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		
Result description
~~~~~~~~~~~~~~~~~~
	A mbedtls ecdhe example thread will be started automatically when booting.

	[Expected Output]

	Seeding the random number generator... ok
	Setting up client context...

	init_thread(55), Available heap 0x3fc8b0 ok
		. Setting up server context... ok
		. Server reading client key and computing secret... ok
		. Client reading server key and computing secret... ok
		. Checking if both computed secrets are equal... ok


Supported List
~~~~~~~~~~~~~~
[Supported List]
        Supported IC :
                Ameba-z2