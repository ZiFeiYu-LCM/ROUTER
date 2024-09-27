#ifndef SETIP_H
#define SETIP_H

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <error.h>
#include <net/route.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <linux/if_arp.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <ifaddrs.h>
#include "../tojson.h"
#include "../common.h"
uint32_t cidr_to_mask(int cidr);
void mask_to_str(uint32_t mask, char *buf);

char* setIpAddr(void *arg);
char* get_ip_address(void *arg);

#endif
