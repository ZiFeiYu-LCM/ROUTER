#ifndef ROUTE_H
#define ROUTE_H
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include "../tojson.h"
#include "../common.h"
#define MAX_ROUTE_INFO 1024
#define ROUTER_VAR_PATH "/var/routers.tmp"
#define ALLROUTER 0
#define STATICROUTER 1
#define LINKROUTER 2
#define RIPROUTER 3
#define OSPFROUTER 4
#define BGPROUTER 5






void* routeShow(void *param);
void* hostShow();
void* staticRouteShow();
void* addStaticRoute(void* data);
#endif
