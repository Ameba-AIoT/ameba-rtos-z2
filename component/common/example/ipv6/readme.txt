IPV6 EXAMPLE

Description:
Example for IPV6.

Configuration:
[lwipopts.h]
    #define LWIP_IPV6                       1
[platform_opts.h]
    #define CONFIG_EXAMPLE_IPV6             1

[example_ipv6.h]
#define UDP_SERVER_IP    "fe80:0000:0000:0000:cd3c:24de:386d:9ad1"
#define TCP_SERVER_IP    "fe80:0000:0000:0000:cd3c:24de:386d:9ad1"
Change the ipv6 address according to your own device.

[example_ipv6.c]  Users can only enable one example at one time.
    example_ipv6_udp_server();
//  example_ipv6_tcp_server();
//  example_ipv6_mcast_server();
//  example_ipv6_udp_client();
//  example_ipv6_tcp_client();
//  example_ipv6_mcast_client();

Execution:
1. Requires a router with IPv6 enabled
2. Requires two boards, one as client and another one as server.
3. Can make automatical Wi-Fi connection when booting by using wlan fast connect example.
4. A IPV6 example thread will be started automatically when booting.

[LwIP Version]
    Verified LwIP Version:
        lwip-2.0.2, lwip-2.1.2

[Supported List]
    Supported IC:
        Ameba-z2