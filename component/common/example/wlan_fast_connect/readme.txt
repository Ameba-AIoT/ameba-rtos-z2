WLAN FAST CONNECT EXAMPLE

Description:
Example for wlan fast connect example

Configuration:

[platform_opts.h]
    #define CONFIG_EXAMPLE_WLAN_FAST_CONNECT            1

Execution:
First, manual connect the board by setting the SSID and Password through AT commands

ATW0=SSID_Name
ATW1=Password
ATWC

Wait until it connects, then reset the board and it will automatically connect to the same WLAN

[Supported List]
	Supported :
	    Ameba-1, Ameba-z, Ameba-pro, Ameba-z2