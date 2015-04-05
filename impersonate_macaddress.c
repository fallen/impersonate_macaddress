/*
 * COPYRIGHT (C) 2015 Yann Sionneau <ys@m-labs.hk>
 */ 

#define _GNU_SOURCE

#include <net/if.h>
#include <net/if_arp.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>

#define ETHER_ADDR_LEN 6

int ioctl(int d, unsigned long request, ...)
{
	va_list myargs;
	struct ifreq *ifr;
	int ret;
	int i;
	char mac_addr_buff[ETHER_ADDR_LEN];
	char *mac_addr;

	va_start(myargs, request);
	mac_addr = getenv("MACADDR");
	sscanf(mac_addr, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx", &mac_addr_buff[0],
           &mac_addr_buff[1], &mac_addr_buff[2], &mac_addr_buff[3],
           &mac_addr_buff[4], &mac_addr_buff[5]);

	if(request == SIOCGIFHWADDR) {
		ifr = va_arg(myargs, struct ifreq *);
		ifr->ifr_hwaddr.sa_family = ARPHRD_ETHER;
		for(i = 0; i < ETHER_ADDR_LEN; i++)
			ifr->ifr_hwaddr.sa_data[i] = mac_addr_buff[i];
		va_end(myargs);
		return 0;
	} else {
		int (*real_ioctl)(int, unsigned long, ...);
		real_ioctl = dlsym(RTLD_NEXT, "ioctl");
		ret = real_ioctl(d, request, myargs);
		va_end(myargs);
		return ret;
	}
}
