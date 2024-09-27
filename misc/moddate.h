#ifndef MODDATE
#define MODDATE

#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <regex.h>
#include "../common.h"
#include "../tojson.h"


void* setDate(void *arg);
void* getDate(void *arg);
void* setTimezone(void *arg);


void* startNTP(void *arg);
void* stopNTP(void *arg);

void* getNTPinfo(void *arg);
void* setNTPinfo(void *arg);
void* delNTPServer(void *arg);

void* setsntpinfo(void *arg);
void* getsntpinfo(void *arg);
#endif
