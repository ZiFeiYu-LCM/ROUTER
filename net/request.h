#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "../tojson.h"
#include "../common.h"
#define BASEURL "https://47ffd038-3f8b-4e9b-beae-e4dd98ca2886.mock.pstmn.io"
#define GETMPLISTURL "192.168.3.100"



typedef struct _memoryStruct {
	char *memory;
	size_t size;
}memStruct;

size_t writeMemoryCallback(void *content,size_t size,size_t nmemb,void *userp);
char* curlRquest(char *url, char *data, size_t(*callBack)(void*,size_t,size_t,void*), char isGet);
char* _curlPost(char *url, char *data, size_t(*callBack)(void*,size_t,size_t,void*));
char* _curlGet(char *url, char *data, size_t(*callBack)(void*,size_t,size_t,void*));



char* curlPost(char *url, char *data);
char* curlGet(char *url, char *data);

//cmd调用
void* getMpList(void **data);
