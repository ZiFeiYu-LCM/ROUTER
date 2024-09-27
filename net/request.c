#include "request.h"

size_t writeMemoryCallback(void *content,size_t size,size_t nmemb,void *userp){
	size_t realsize = size*nmemb;
	memStruct *mem = (memStruct*)userp;
	char *ptr = realloc(mem->memory,mem->size+1+realsize);
	if(!ptr){
		printf("failed to realloc\n");
		return 0;
	}
	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]),content,realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;
	return realsize;
}


char* curlRquest(char *url, char *data, size_t(*callBack)(void*,size_t,size_t,void*), char isGet){
	CURL *curl_handle;
	CURLcode res;
	struct curl_slist *headers = NULL;
	memStruct chunk;
	chunk.memory = malloc(1);
	chunk.size = 0;
	
	char *tmp;
	if(isGet && data!=NULL){
		tmp = (char*)malloc(strlen(url)+strlen(data)+2); //用于get请求地址
		printf("2.0.0\n");
		strcat(tmp,url);
		strcat(tmp,"?");
		strcat(tmp,data);
		url = tmp;
		printf("geturl: %s\n",url);	
	}
	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_URL,url);
	if(!isGet){//POST
		curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, data);
	}
	headers = curl_slist_append(headers, "Content-Type: application/json");
	curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, callBack);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA,(void*)&chunk);
	//curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
	res = curl_easy_perform(curl_handle);
	if(res!=CURLE_OK){
		fprintf(stderr,"curl_easy_perform() failed:%s\n",curl_easy_strerror(res));
		return NULL;
	}
	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();
	if(isGet && data!=NULL)
		free(tmp);
	
	//将相应结构体中的data字段提出来
	cJSON *json = cJSON_Parse(chunk.memory);
	if (json == NULL) {
		free(chunk.memory);
        printf("Error parsing JSON\n");
        return 1;
    }
	cJSON *resData = cJSON_GetObjectItem(json, "data");
    if (resData == NULL) {
        printf("Error extracting data\n");
        cJSON_Delete(json);
		free(chunk.memory);
        return 1;
    }
	//printf("chunk.memory len: %d\n",strlen(chunk.memory));
	//char *data_string = (char*)malloc(strlen(chunk.memory));
	char *data_string = cJSON_Print(resData);
    if (data_string == NULL) {
        printf("Error printing data\n");
        cJSON_Delete(json);
		free(chunk.memory);
        return 1;
    }
	
	
	
	free(chunk.memory);
	cJSON_Delete(json);
	
	//printf("%s\n",data_string);
	return data_string;

}

char* _curlPost(char *url, char *data, size_t(*callBack)(void*,size_t,size_t,void*)){
	return curlRquest(url,data,callBack,0);
	
}

char* _curlGet(char *url, char *data, size_t(*callBack)(void*,size_t,size_t,void*)){
	return curlRquest(url,data,callBack,1);
}

//外部调用
char* curlPost(char *url, char *data){
	return _curlPost(url,data,writeMemoryCallback);
}
char* curlGet(char *url, char *data){
	return _curlGet(url,data,writeMemoryCallback);
}

void* getMpList(void **data){
	char url[100];
	sprintf(url,"%s/%s",BASEURL,"test");
	*data = (void*)curlGet(url,NULL);
	return data;
}




/*
int main(){
//	char *res = curlGet(REQUESTCURL,"key=12",writeMemoryCallback);
//	printf("response:%s\n",res);
//	free(res);
	char *res = curlPost(REQUESTCURL,NULL,writeMemoryCallback);
	printf("response:%s\n",res);
	free(res);
	return 0;
}
*/
