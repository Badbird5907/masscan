/*
    retrieve IPv4 address of the named network interface/adapter
    like "eth0"


    This works on:
        - Windows
        - Linux
        - Apple
        - FreeBSD

 I think it'll work the same on any BSD system.
*/
#include "rawsock.h"
#include "string_s.h"
#include "massip-parse.h"

/*****************************************************************************
 *****************************************************************************/
#if defined(__linux__)
#define _GNU_SOURCE
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_link.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

ipv6address
rawsock_get_adapter_ipv6(const char *ifname)
{
    struct ifaddrs *list = NULL;
    struct ifaddrs *ifa;
    int err;
    ipv6address result = {0,0};

    /* Fetch the list of addresses */
    err = getifaddrs(&list);
    if (err == -1) {
        fprintf(stderr, "[-] getifaddrs(): %s\n", strerror(errno));
        return result;
    }

    for (ifa = list; ifa != NULL; ifa = ifa->ifa_next) {
        ipv6address addr;

        if (ifa->ifa_addr == NULL)
            continue;
        if (ifa->ifa_name == NULL)
            continue;
        if (strcmp(ifname, ifa->ifa_name) != 0)
            continue;
        if (ifa->ifa_addr->sa_family != AF_INET6)
            continue;

        addr = ipv6address_from_bytes((const unsigned char *)&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr);
        
        if (addr.hi>>56ULL >= 0xFC)
            continue;
        if (addr.hi>>32ULL == 0x20010db8)
            continue;

        result = addr;
        break;
    }

    freeifaddrs(list);
    return result;
}

/*****************************************************************************
 *****************************************************************************/
#elif defined(WIN32)
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>
#ifdef _MSC_VER
#pragma comment(lib, "IPHLPAPI.lib")
#endif

ipv6address
rawsock_get_adapter_ipv6(const char *ifname) {
    ipv6address result = {0,0};
    return result;
}

/*****************************************************************************
 *****************************************************************************/
#elif defined(__APPLE__) || defined(__FreeBSD__) || defined(__NetBSD__) || 1
#include <sys/types.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#ifdef AF_LINK
#   include <net/if_dl.h>
#endif
#ifdef AF_PACKET
#   include <netpacket/packet.h>
#endif

ipv6address
rawsock_get_adapter_ipv6(const char *ifname)
{
    struct ifaddrs *list = NULL;
    struct ifaddrs *ifa;
    int err;
    ipv6address result = {0,0};

    /* Fetch the list of addresses */
    err = getifaddrs(&list);
    if (err == -1) {
        fprintf(stderr, "[-] getifaddrs(): %s\n", strerror(errno));
        return result;
    }

    for (ifa = list; ifa != NULL; ifa = ifa->ifa_next) {
        ipv6address addr;

        if (ifa->ifa_addr == NULL)
            continue;
        if (ifa->ifa_name == NULL)
            continue;
        if (strcmp(ifname, ifa->ifa_name) != 0)
            continue;
        if (ifa->ifa_addr->sa_family != AF_INET6)
            continue;

        addr = ipv6address_from_bytes((const unsigned char *)&((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr);
        
        if (addr.hi>>56ULL >= 0xFC)
            continue;
        if (addr.hi>>32ULL == 0x20010db8)
            continue;

        result = addr;
        break;
    }

    freeifaddrs(list);
    return result;
}

#endif

