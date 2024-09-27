#ifndef DHCP_H
#define DHCP_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include "../tojson.h"
#include "../common.h"

#define MAXLENGTH 1024


//dhcp
char* dhcpstart(void *arg);
char* dhcpstop(void *arg);
char* setiprange(void *arg);
char* getdhcpinfo(void *arg);


//dns
/*
char* setDnsDomain(void *arg);
char* setDnsRelay(void *arg);
char* relaystart(void *arg);
char* relaystop(void *arg);

char* delRelay(void *arg);
char* getRelay(void *arg);
*/
#endif
