#ifndef FIREWALL_H

#include <stdio.h>
#include "../tojson.h"
#include "../common.h"
void *addFirewallRule(void* arg);
void *delFirewallRule(void* arg);
char *getFirewallRule(void* arg);

#endif