#include <lwip/sockets.h>
#include <lwip/netif.h>
extern struct netif xnetif[];
extern uint8_t *LwIP_GetIP(struct netif *pnetif);

/*-----------------------------------------------------------------------
 * Mandatory functions
 *-----------------------------------------------------------------------*/

// Mandatory function for custom initialization
// called when mDNS initialization
void mDNSPlatformCustomInit(void)
{
	xnetif[0].flags |= NETIF_FLAG_IGMP;
}

uint16_t mDNSPlatformHtons(uint16_t hostshort)
{
	return htons(hostshort);
}

uint32_t mDNSPlatformInetAddr(char *cp)
{
	return inet_addr(cp);
}

// Mandatory function to get hostname
// called when mDNS initialization
char *mDNSPlatformHostname(void)
{
#if LWIP_NETIF_HOSTNAME
	return (char *) xnetif[0].hostname;
#else
	return "ameba";
#endif
}

/*-----------------------------------------------------------------------
 * WEAK functions
 *-----------------------------------------------------------------------*/


uint8_t mDNSPlatformNetifIsUp(int idx)
{
	return netif_is_up(&xnetif[idx]);
}

#if LWIP_IPV6
uint8_t *mDNSPlatformIP6Addr(int idx)
{
#if LWIP_VERSION_MAJOR >= 2
	return (uint8_t *) & (xnetif[idx].ip6_addr[0].u_addr.ip6);
#else
	return (uint8_t *) & (xnetif[idx].ip6_addr[0].addr[0]);
#endif
}
#endif

char *mDNSPlatformIP1String(int type)
{
	static char ip_str[40];

	if (type == AF_INET) {
		uint8_t *ip = LwIP_GetIP(&xnetif[1]);
		sprintf(ip_str, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
	}

	return ip_str;
}

