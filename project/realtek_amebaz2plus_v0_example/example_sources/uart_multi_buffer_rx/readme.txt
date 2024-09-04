##################################################################################
#                                                                                #
#                          Example uart_multi_buffer_rx                          #
#                                                                                #
##################################################################################

Date: 2023-08-25

Table of Contents
~~~~~~~~~~~~~~~~~
 - Description
 - Setup Guide
 - Result description
 - Supported List

 
Description
~~~~~~~~~~~
        Here gives an example how to do UART Receive with multi buffer in AMEBA-Z2( RTL8710C).
	By using multiple buffer instead of a single buffer to receive data. The data can be
	held on and sent to a queue. Processor can process it later when its free.

	By using this method, it is able to tackle the issue when uart sends data faster 
	than the processer can process data.
        
	Please set baudrate to 115200

Setup Guide
~~~~~~~~~~~
	Copy <sdk>\project\realtek_amebaz2_v0_example\example_sources\uart_multi_buffer_rx\src\main.c to
	<sdk>\project\realtek_amebaz2_v0_example\src.
        Build and flash to AMEBA-Z2 board.
	Connect TTL cable to PA13 (RX) and PA14 (TX), Open port in Tera Term and start sending.
	Set Tera Term baudrate to 115200
	NOTE: You can use the send file feature in Tera Term to dump data into the uart rx.
	
Result description
~~~~~~~~~~~~~~~~~~
	You should be able to see all characters being received and printed out in hex & a counter to
	keep track of the number of bytes received.

Supported List
~~~~~~~~~~~~~~
[Supported List]
        Supported :
           Ameba-z2,