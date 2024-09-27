#include "tojson.h"


int jsonToSshinfo(char *jsonStr,sshinfo **data){
	cJSON *jsonArray = cJSON_Parse(jsonStr);
	if(jsonArray==NULL || jsonStr==NULL){
		LOG_ERROR("failed to create jsonToRouteinfo");
		return 0;
	}
	if(!cJSON_IsArray(jsonArray)){
		cJSON_Delete(jsonArray);
		return -1;
	}
	size_t arraySize = cJSON_GetArraySize(jsonArray);
	*data = (firewallrule*)malloc(sizeof(firewallrule)*arraySize);
	for(int i=0;i<arraySize;i++){
		cJSON *json = cJSON_GetArrayItem(jsonArray,i);

		cJSON *listenaddr = cJSON_GetObjectItemCaseSensitive(json,"listenaddr");
		if(cJSON_IsString(listenaddr)&&(listenaddr->valuestring!=NULL)){
			strcpy((*data)[i].listenaddr,listenaddr->valuestring);
		}		
		cJSON *port = cJSON_GetObjectItemCaseSensitive(json,"port");
		if(cJSON_IsString(port)&&(port->valuestring!=NULL)){
			strcpy((*data)[i].port,port->valuestring);
		}			
		cJSON *outtime = cJSON_GetObjectItemCaseSensitive(json,"outtime");
		if(cJSON_IsString(outtime)&&(outtime->valuestring!=NULL)){
			strcpy((*data)[i].outtime,outtime->valuestring);
		}
		
		cJSON_AddItemToArray(json,jsonArray);
	}
	cJSON_Delete(jsonArray);
	return arraySize;
}
char* sshinfoToJson(sshinfo* data,int size){
	cJSON *jsonArray = cJSON_CreateArray();
	if(jsonArray == NULL){
		LOG_ERROR("failed to create cJSON_CreateArray");
		return NULL;
	}
	for(int i=0;i<size;i++){
		cJSON *jsonItem = cJSON_CreateObject();
		cJSON_AddStringToObject(jsonItem,"listenaddr",data->listenaddr);
		cJSON_AddStringToObject(jsonItem,"port",data->port);
		cJSON_AddStringToObject(jsonItem,"outtime",data->outtime);
		cJSON_AddItemToArray(jsonArray,jsonItem);
	}
	char *str = cJSON_Print(jsonArray);
	cJSON_Delete(jsonArray);
	return str;
}


int jsonToDnsinfo(char *jsonStr,dnsinfo **data){
	cJSON *jsonArray = cJSON_Parse(jsonStr);
	if(jsonArray==NULL || jsonStr==NULL){
		LOG_ERROR("failed to create jsonToRouteinfo");
		return 0;
	}
	if(!cJSON_IsArray(jsonArray)){
		cJSON_Delete(jsonArray);
		return -1;
	}
	size_t arraySize = cJSON_GetArraySize(jsonArray);
	*data = (firewallrule*)malloc(sizeof(firewallrule)*arraySize);
	for(int i=0;i<arraySize;i++){
		cJSON *json = cJSON_GetArrayItem(jsonArray,i);

		cJSON *domain1 = cJSON_GetObjectItemCaseSensitive(json,"domain1");
		if(cJSON_IsString(domain1)&&(domain1->valuestring!=NULL)){
			strcpy((*data)[i].domain1,domain1->valuestring);
		}		
		cJSON *domain2 = cJSON_GetObjectItemCaseSensitive(json,"domain2");
		if(cJSON_IsString(domain2)&&(domain2->valuestring!=NULL)){
			strcpy((*data)[i].domain2,domain2->valuestring);
		}				
		cJSON *ip1 = cJSON_GetObjectItemCaseSensitive(json,"ip1");
		if(cJSON_IsString(ip1)&&(ip1->valuestring!=NULL)){
			strcpy((*data)[i].ip1,ip1->valuestring);
		}
		cJSON *ip2 = cJSON_GetObjectItemCaseSensitive(json,"ip2");
		if(cJSON_IsString(ip2)&&(ip2->valuestring!=NULL)){
			strcpy((*data)[i].ip2,ip2->valuestring);
		}
		cJSON_AddItemToArray(json,jsonArray);
	}
	cJSON_Delete(jsonArray);
	return arraySize;	
}

char* dnsinfoToJson(dnsinfo* data,int size){
	cJSON *jsonArray = cJSON_CreateArray();
	if(jsonArray == NULL){
		LOG_ERROR("failed to create cJSON_CreateArray");
		return NULL;
	}
	for(int i=0;i<size;i++){
		cJSON *jsonItem = cJSON_CreateObject();
		cJSON_AddStringToObject(jsonItem,"domain1",data->domain1);
		cJSON_AddStringToObject(jsonItem,"domain2",data->domain2);
		cJSON_AddStringToObject(jsonItem,"ip1",data->ip1);
		cJSON_AddStringToObject(jsonItem,"ip2",data->ip2);
		cJSON_AddItemToArray(jsonArray,jsonItem);
	}
	char *str = cJSON_Print(jsonArray);
	cJSON_Delete(jsonArray);
	return str;
}


int jsonToNtpserver(char *jsonStr,ntpserver **data){
	printf("jsonToNtpserver:     %s\n",jsonStr);
	if(jsonStr==NULL){
		LOG_ERROR("failed to create jsonToRouteinfo");
		return 0;
	}
	cJSON *jsonArray = cJSON_Parse(jsonStr);
	if(!cJSON_IsArray(jsonArray)){
		cJSON_Delete(jsonArray);
		return -1;
	}
	size_t arraySize = cJSON_GetArraySize(jsonArray);
	*data = (ntpserver*)malloc(sizeof(ntpserver)*arraySize);	
	for(int i=0;i<arraySize;i++){
		cJSON *json = cJSON_GetArrayItem(jsonArray,i);
		cJSON *server = cJSON_GetObjectItemCaseSensitive(json,"server");
		if(cJSON_IsString(server)&&(server->valuestring!=NULL)){
			strcpy((*data)[i].server,server->valuestring);
		}
		cJSON *port = cJSON_GetObjectItemCaseSensitive(json,"port");
		if(cJSON_IsString(port)&&(port->valuestring!=NULL)){
			strcpy((*data)[i].port,port->valuestring);
		}
		cJSON *ismain = cJSON_GetObjectItemCaseSensitive(json,"ismain");
		if(cJSON_IsNumber(ismain)){
			(*data)[i].ismain=ismain->valueint;
		}	
	}
	cJSON_Delete(jsonArray);
	return arraySize;
}
char* ntpserverToJson(ntpserver* data,int size){
	cJSON *jsonArray = cJSON_CreateArray();
	if(jsonArray == NULL){
		LOG_ERROR("failed to create cJSON_CreateArray");
		return NULL;
	}
	for(int i=0;i<size;i++){
		cJSON *jsonItem = cJSON_CreateObject();
		cJSON_AddStringToObject(jsonItem,"server",data->server);
		cJSON_AddNumberToObject(jsonItem,"ismain",data->ismain);
		cJSON_AddStringToObject(jsonItem,"port",data->port);
		cJSON_AddItemToArray(jsonArray,jsonItem);
	}
	char *str = cJSON_Print(jsonArray);
	cJSON_Delete(jsonArray);
//	printf("ntpserverToJson: %s\n",str);
	return str;	
}
int jsonToNtpinfo(char *jsonStr,ntpinfo **data){
	cJSON *jsonArray = cJSON_Parse(jsonStr);
	if(jsonArray==NULL || jsonStr==NULL){
		LOG_ERROR("failed to create jsonToRouteinfo");
		return 0;
	}
	if(!cJSON_IsArray(jsonArray)){
		cJSON_Delete(jsonArray);
		return -1;
	}
	size_t arraySize = cJSON_GetArraySize(jsonArray);
	*data = (ntpinfo*)malloc(sizeof(ntpinfo)*arraySize);
	for(int i=0;i<arraySize;i++){
		cJSON *json = cJSON_GetArrayItem(jsonArray,i);
		
		cJSON *servers = cJSON_GetObjectItemCaseSensitive(json,"servers");		
		jsonToNtpserver(cJSON_Print(servers),&((*data)[i].servers));

		cJSON *updatetime = cJSON_GetObjectItemCaseSensitive(json,"updatetime");
		if(cJSON_IsString(updatetime)&&(updatetime->valuestring!=NULL)){
			strcpy((*data)[i].updatetime,updatetime->valuestring);
		}
		cJSON *interface = cJSON_GetObjectItemCaseSensitive(json,"interface");
		if(cJSON_IsString(interface)&&(interface->valuestring!=NULL)){
			strcpy((*data)[i].interface,interface->valuestring);
		}
		cJSON *srcaddr = cJSON_GetObjectItemCaseSensitive(json,"srcaddr");
		if(cJSON_IsString(srcaddr)&&(srcaddr->valuestring!=NULL)){
			strcpy((*data)[i].srcaddr,srcaddr->valuestring);
		}
		cJSON *maxpoll = cJSON_GetObjectItemCaseSensitive(json,"maxpoll");
		if(cJSON_IsNumber(maxpoll)){
			(*data)[i].maxpoll=maxpoll->valueint;
		}	
		cJSON *minpoll = cJSON_GetObjectItemCaseSensitive(json,"minpoll");
		if(cJSON_IsNumber(minpoll)){
			(*data)[i].minpoll=minpoll->valueint;
		}	
		cJSON *serverNum = cJSON_GetObjectItemCaseSensitive(json,"serverNum");
		if(cJSON_IsNumber(serverNum)){
			(*data)[i].serverNum=serverNum->valueint;
		}	
	}
	cJSON_Delete(jsonArray);
	return arraySize;
}

char* ntpinfoToJson(ntpinfo* data,int size){
	cJSON *jsonArray = cJSON_CreateArray();
	if(jsonArray == NULL){
		LOG_ERROR("failed to create cJSON_CreateArray");
		return NULL;
	}
	for(int i=0;i<size;i++){
		cJSON *jsonItem = cJSON_CreateObject();
		cJSON_AddStringToObject(jsonItem,"servers",ntpserverToJson(data->servers,data->serverNum));
		cJSON_AddStringToObject(jsonItem,"updatetime",data->updatetime);
		cJSON_AddStringToObject(jsonItem,"interface",data->interface);
		cJSON_AddStringToObject(jsonItem,"srcaddr",data->srcaddr);
		cJSON_AddNumberToObject(jsonItem,"maxpoll",data->maxpoll);
		cJSON_AddNumberToObject(jsonItem,"minpoll",data->minpoll);
		cJSON_AddNumberToObject(jsonItem,"serverNum",data->serverNum);
		cJSON_AddItemToArray(jsonArray,jsonItem);
	}
	char *str = cJSON_Print(jsonArray);
	cJSON_Delete(jsonArray);
	return str;	
}

int jsonToHostinfo(char *jsonStr,hostinfo **data){
	cJSON *jsonArray = cJSON_Parse(jsonStr);
	if(jsonArray==NULL || jsonStr==NULL){
		LOG_ERROR("failed to create jsonToRouteinfo");
		return 0;
	}
	if(!cJSON_IsArray(jsonArray)){
		cJSON_Delete(jsonArray);
		return -1;
	}
	size_t arraySize = cJSON_GetArraySize(jsonArray);
	*data = (firewallrule*)malloc(sizeof(firewallrule)*arraySize);
	for(int i=0;i<arraySize;i++){
		cJSON *json = cJSON_GetArrayItem(jsonArray,i);

		cJSON *interface = cJSON_GetObjectItemCaseSensitive(json,"interface");
		if(cJSON_IsString(interface)&&(interface->valuestring!=NULL)){
			strcpy((*data)[i].interface,interface->valuestring);
		}		
		cJSON *mac = cJSON_GetObjectItemCaseSensitive(json,"mac");
		if(cJSON_IsString(mac)&&(mac->valuestring!=NULL)){
			strcpy((*data)[i].mac,mac->valuestring);
		}				
		cJSON *ip = cJSON_GetObjectItemCaseSensitive(json,"ip");
		if(cJSON_IsString(ip)&&(ip->valuestring!=NULL)){
			strcpy((*data)[i].ip,ip->valuestring);
		}
		cJSON *name = cJSON_GetObjectItemCaseSensitive(json,"name");
		if(cJSON_IsString(name)&&(name->valuestring!=NULL)){
			strcpy((*data)[i].name,name->valuestring);
		}
		cJSON *validtime = cJSON_GetObjectItemCaseSensitive(json,"validtime");
		if(cJSON_IsString(validtime)&&(validtime->valuestring!=NULL)){
			strcpy((*data)[i].validtime,validtime->valuestring);
		}
		cJSON_AddItemToArray(json,jsonArray);
	}
	cJSON_Delete(jsonArray);
	return arraySize;
}

char* hostinfoToJson(hostinfo* data,int size){
	cJSON *jsonArray = cJSON_CreateArray();
	if(jsonArray == NULL){
		LOG_ERROR("failed to create cJSON_CreateArray");
		return NULL;
	}
	for(int i=0;i<size;i++){
		cJSON *jsonItem = cJSON_CreateObject();
		cJSON_AddStringToObject(jsonItem,"interface",data->interface);
		cJSON_AddStringToObject(jsonItem,"mac",data->mac);
		cJSON_AddStringToObject(jsonItem,"ip",data->ip);
		cJSON_AddStringToObject(jsonItem,"name",data->name);
		cJSON_AddStringToObject(jsonItem,"validtime",data->validtime);
		cJSON_AddItemToArray(jsonArray,jsonItem);
	}
	char *str = cJSON_Print(jsonArray);
	cJSON_Delete(jsonArray);
	return str;
}

int jsonToFirewallrule(char *jsonStr,firewallrule **data){
	cJSON *jsonArray = cJSON_Parse(jsonStr);
	if(jsonArray==NULL || jsonStr==NULL){
		LOG_ERROR("failed to create jsonToRouteinfo");
		return 0;
	}
	if(!cJSON_IsArray(jsonArray)){
		cJSON_Delete(jsonArray);
		return -1;
	}
	size_t arraySize = cJSON_GetArraySize(jsonArray);
	*data = (firewallrule*)malloc(sizeof(firewallrule)*arraySize);
	printf("fire 1\n");
	for(int i=0;i<arraySize;i++){
		cJSON *json = cJSON_GetArrayItem(jsonArray,i);
		cJSON *id = cJSON_GetObjectItemCaseSensitive(json,"id");
		if(cJSON_IsNumber(id)){
			(*data)[i].id = id->valueint;
		}
		cJSON *table = cJSON_GetObjectItemCaseSensitive(json,"table");
		if(cJSON_IsString(table)&&(table->valuestring!=NULL)){
			strcpy((*data)[i].table,table->valuestring);
		}
		cJSON *protocol = cJSON_GetObjectItemCaseSensitive(json,"protocol");
		if(cJSON_IsString(protocol)&&(protocol->valuestring!=NULL)){
			strcpy((*data)[i].protocol,protocol->valuestring);
		}		
//		cJSON *addordel = cJSON_GetObjectItemCaseSensitive(json,"addordel");
//		if(cJSON_IsNumber(addordel)){
//			(*data)[i].addordel = addordel->valueint;
//		}			
		cJSON *srcip = cJSON_GetObjectItemCaseSensitive(json,"srcip");
		if(cJSON_IsString(srcip)&&(srcip->valuestring!=NULL)){
			strcpy((*data)[i].srcip,srcip->valuestring);
		}
		cJSON *desip = cJSON_GetObjectItemCaseSensitive(json,"desip");
		if(cJSON_IsString(desip)&&(desip->valuestring!=NULL)){
			strcpy((*data)[i].desip,desip->valuestring);
		}
		cJSON *port = cJSON_GetObjectItemCaseSensitive(json,"port");
		if(cJSON_IsString(port)&&(port->valuestring!=NULL)){
			strcpy((*data)[i].port,port->valuestring);
		}
		cJSON *in_interface = cJSON_GetObjectItemCaseSensitive(json,"in_interface");
		if(cJSON_IsString(in_interface)&&(in_interface->valuestring!=NULL)){
			strcpy((*data)[i].in_interface,in_interface->valuestring);
		}
		cJSON *out_interface = cJSON_GetObjectItemCaseSensitive(json,"out_interface");
		if(cJSON_IsString(out_interface)&&(out_interface->valuestring!=NULL)){
			strcpy((*data)[i].out_interface,out_interface->valuestring);
		}
		cJSON *jump = cJSON_GetObjectItemCaseSensitive(json,"jump");
		if(cJSON_IsString(jump)&&(jump->valuestring!=NULL)){
			strcpy((*data)[i].jump,jump->valuestring);
		}	
	}
	printf("fire 2\n");
	cJSON_Delete(jsonArray);
	printf("fire 3\n");
	return arraySize;
}

char* firewallruleToJson(firewallrule* data,int size){
	cJSON *jsonArray = cJSON_CreateArray();
	if(jsonArray == NULL){
		LOG_ERROR("failed to create cJSON_CreateArray");
		return NULL;
	}
	for(int i=0;i<size;i++){
		cJSON *jsonItem = cJSON_CreateObject();
		cJSON_AddNumberToObject(jsonItem,"id",data->id);
//		cJSON_AddStringToObject(jsonItem,"addordel",data->addordel);
		cJSON_AddStringToObject(jsonItem,"table",data->table);
		cJSON_AddStringToObject(jsonItem,"protocol",data->protocol);
		cJSON_AddStringToObject(jsonItem,"srcip",data->srcip);
		cJSON_AddStringToObject(jsonItem,"desip",data->desip);
		cJSON_AddStringToObject(jsonItem,"port",data->port);
		cJSON_AddStringToObject(jsonItem,"in_interface",data->in_interface);
		cJSON_AddStringToObject(jsonItem,"out_interface",data->out_interface);
		cJSON_AddStringToObject(jsonItem,"jump",data->jump);
		cJSON_AddItemToArray(jsonArray,jsonItem);
	}
	char *str = cJSON_Print(jsonArray);
	cJSON_Delete(jsonArray);
	return str;
}

int jsonToIpinfo(char *jsonStr,ipinfo **data){
	cJSON *jsonArray = cJSON_Parse(jsonStr);
	if(jsonArray==NULL || jsonStr==NULL){
		LOG_ERROR("failed to create jsonToRouteinfo");
		return 0;
	}
	if(!cJSON_IsArray(jsonArray)){
		cJSON_Delete(jsonArray);
		return -1;
	}
	size_t arraySize = cJSON_GetArraySize(jsonArray);
	*data = (ipinfo*)malloc(sizeof(ipinfo)*arraySize);
	for(int i=0;i<arraySize;i++){
		cJSON *json = cJSON_GetArrayItem(jsonArray,i);
		cJSON *ifname = cJSON_GetObjectItemCaseSensitive(json,"ifname");
		if(cJSON_IsString(ifname)&&(ifname->valuestring!=NULL)){
			strcpy((*data)[i].ifname,ifname->valuestring);
		}
		cJSON *ipaddr = cJSON_GetObjectItemCaseSensitive(json,"ipaddr");
		if(cJSON_IsString(ipaddr)&&(ipaddr->valuestring!=NULL)){
			strcpy((*data)[i].ipaddr,ipaddr->valuestring);
		}
		cJSON *mask = cJSON_GetObjectItemCaseSensitive(json,"mask");
		if(cJSON_IsString(mask)&&(mask->valuestring!=NULL)){
			strcpy((*data)[i].mask,mask->valuestring);
		}			
		cJSON *gateway = cJSON_GetObjectItemCaseSensitive(json,"gateway");
		if(cJSON_IsString(gateway)&&(gateway->valuestring!=NULL)){
			strcpy((*data)[i].gateway,gateway->valuestring);
		}
		cJSON *mac_str = cJSON_GetObjectItemCaseSensitive(json,"mac_str");
		if(cJSON_IsString(mac_str)&&(mac_str->valuestring!=NULL)){
			strcpy((*data)[i].mac_str,mac_str->valuestring);
		}
	}
	
	cJSON_Delete(jsonArray);
	return arraySize;
	
}

char* ipinfoToJson(ipinfo* data,int size){
	cJSON *jsonArray = cJSON_CreateArray();
	if(jsonArray == NULL){
		LOG_ERROR("failed to create cJSON_CreateArray");
		return NULL;
	}
	
	for(int i=0;i<size;i++){
		cJSON *jsonItem = cJSON_CreateObject();
		cJSON_AddStringToObject(jsonItem,"ifname",data->ifname);
		cJSON_AddStringToObject(jsonItem,"ipaddr",data->ipaddr);
		cJSON_AddStringToObject(jsonItem,"mask",data->mask);
		cJSON_AddStringToObject(jsonItem,"gateway",data->gateway);
		cJSON_AddStringToObject(jsonItem,"mac_str",data->mac_str);
		
		cJSON_AddItemToArray(jsonArray,jsonItem);
	}
	
	char *str = cJSON_Print(jsonArray);
	cJSON_Delete(jsonArray);
	return str;
}



int jsonToRouteinfo(char* jsonStr,routeInfo** data){
	cJSON *jsonArray = cJSON_Parse(jsonStr);
	if(jsonArray==NULL || jsonStr==NULL){
		LOG_ERROR("failed to create jsonToRouteinfo");
		return 0;
	}
	if(!cJSON_IsArray(jsonArray)){
		cJSON_Delete(jsonArray);
		return -1;
	}
	
	size_t arraySize = cJSON_GetArraySize(jsonArray);
	*data = (routeInfo*)malloc(sizeof(routeInfo)*arraySize);
	for(int i=0;i<arraySize;i++){
		cJSON *json = cJSON_GetArrayItem(jsonArray,i);
		cJSON *ip = cJSON_GetObjectItemCaseSensitive(json,"ip");
		if(cJSON_IsString(ip)&&(ip->valuestring!=NULL)){
			strcpy((*data)[i].ip,ip->valuestring);
		}
		cJSON *netmask = cJSON_GetObjectItemCaseSensitive(json,"netmask");
		if(cJSON_IsString(netmask)&&(netmask->valuestring!=NULL)){
			strcpy((*data)[i].netmask,netmask->valuestring);
		}		
		cJSON *interface = cJSON_GetObjectItemCaseSensitive(json,"interface");
		if(cJSON_IsString(interface)&&(interface->valuestring!=NULL)){
			strcpy((*data)[i].interface,interface->valuestring);
		}			
		cJSON *gateway = cJSON_GetObjectItemCaseSensitive(json,"gateway");
		if(cJSON_IsString(gateway)&&(gateway->valuestring!=NULL)){
			strcpy((*data)[i].gateway,gateway->valuestring);
		}
		cJSON *metric = cJSON_GetObjectItemCaseSensitive(json,"metric");
		if(cJSON_IsString(metric)&&(metric->valuestring!=NULL)){
			strcpy((*data)[i].metric,metric->valuestring);
		}
		cJSON *track = cJSON_GetObjectItemCaseSensitive(json,"track");
		if(cJSON_IsString(track)&&(track->valuestring!=NULL)){
			strcpy((*data)[i].track,track->valuestring);
		}
		cJSON *proto = cJSON_GetObjectItemCaseSensitive(json,"proto");
		if(cJSON_IsString(proto)&&(proto->valuestring!=NULL)){
			strcpy((*data)[i].proto,proto->valuestring);
		}			
		cJSON_AddItemToArray(json,jsonArray);
	}
	cJSON_Delete(jsonArray);
	return arraySize;
}

char* routeinfoToJson(routeInfo* data,int size){
	cJSON *jsonArray = cJSON_CreateArray();
	if(jsonArray == NULL){
		LOG_ERROR("failed to create cJSON_CreateArray");
		return NULL;
	}
	
	for(int i=0;i<size;i++){
			cJSON *jsonItem = cJSON_CreateObject();
			
			cJSON_AddStringToObject(jsonItem,"ip",data->ip);
			cJSON_AddStringToObject(jsonItem,"netmask",data->netmask);
			cJSON_AddStringToObject(jsonItem,"interface",data->interface);
			cJSON_AddStringToObject(jsonItem,"gateway",data->gateway);
			cJSON_AddStringToObject(jsonItem,"metric",data->metric);
			cJSON_AddStringToObject(jsonItem,"track",data->track);
			cJSON_AddStringToObject(jsonItem,"proto",data->proto);
			
			cJSON_AddItemToArray(jsonArray,jsonItem);
	}
	
	char *str = cJSON_Print(jsonArray);
	cJSON_Delete(jsonArray);
	return str;
}		

int jsonToDhcpinfo(char *jsonStr,dhcpinfo **data){	
	cJSON *jsonArray = cJSON_Parse(jsonStr);
	if(jsonArray == NULL || jsonStr==NULL || strcmp(jsonStr,"")==0){
		LOG_ERROR("jsonToDhcpinfo error");
		*data=NULL;
		return 0;		
	}
	size_t arraySize = cJSON_GetArraySize(jsonArray);
	*data = (dhcpinfo*)malloc(sizeof(dhcpinfo)*arraySize);
	if(*data == NULL){
		LOG_ERROR("Memory allocation failed");
		cJSON_Delete(jsonArray);
		return -1;
	}
	
	for(int i=0;i<arraySize;i++){
		cJSON *json = cJSON_GetArrayItem(jsonArray,i);
		
		cJSON *interface = cJSON_GetObjectItemCaseSensitive(json, "interface");
		if(cJSON_IsString(interface)&&(interface->valuestring!=NULL)){
			strcpy((*data)[i].interface,interface->valuestring);
		}
		cJSON *iprange_start = cJSON_GetObjectItemCaseSensitive(json, "iprange_start");
		if(cJSON_IsString(iprange_start)&&(iprange_start->valuestring!=NULL)){
			strcpy((*data)[i].iprange_start,iprange_start->valuestring);
		}
		cJSON *iprange_end = cJSON_GetObjectItemCaseSensitive(json, "iprange_end");
		if(cJSON_IsString(iprange_end)&&(iprange_end->valuestring!=NULL)){
			strcpy((*data)[i].iprange_end,iprange_end->valuestring);
		}
		cJSON *validtime = cJSON_GetObjectItemCaseSensitive(json, "validtime");
		if(cJSON_IsNumber(validtime)){
			(*data[i]).validtime = validtime->valueint;
		}
	}
	
	cJSON_Delete(jsonArray);
	return arraySize;  
}

char* dhcpinfoToJson(dhcpinfo* data,int size){
	cJSON *jsonArray = cJSON_CreateArray();
	
	for(int i=0;i<size;i++){
		cJSON *json = cJSON_CreateObject();
		cJSON_AddStringToObject(json,"interface",data[i].interface);
		cJSON_AddStringToObject(json,"iprange_start",data[i].iprange_start);
		cJSON_AddStringToObject(json,"iprange_end",data[i].iprange_end);
		cJSON_AddNumberToObject(json,"validtime",data[i].validtime);
		cJSON_AddItemToArray(jsonArray, json);
	}
	
	char *jsonStr = cJSON_Print(jsonArray);
	cJSON_Delete(jsonArray);
	return jsonStr;
}

/*
int jsonToPostparam(char *jsonStr,postParam **data){
	if(jsonStr==NULL || strcmp(jsonStr,"")==0){
		*data=NULL;
		return 0;
	}
	cJSON *json = cJSON_Parse(jsonStr);
	if(json == NULL){
		*data = NULL;
		return;
	}
	*data = (readMpinfo*)malloc(sizeof(readMpinfo));
	if(*data == NULL){
		LOG_ERROR("Memory allocation failed");
		cJSON_Delete(json);
		return -1;
	}
	cJSON *dataDrray = cJSON_GetObjectItem(json,"data");
	char *data_string = cJSON_Print(dataDrray);
	jsonToReadMpinfo(data_string,&((*data)->data));
	free(data_string);

	cJSON *dataSize = cJSON_GetObjectItemCaseSensitive(json, "dataSize");
	if (cJSON_IsNumber(dataSize)) {
		(*data)->dataSize = dataSize->valueint;
	}

	cJSON_Delete(json);
	return (*data)->dataSize;
}

char* postparamToJson(postParam* data,int size){
	cJSON *jsonArray = cJSON_CreateArray();
	
	cJSON *json = cJSON_CreateObject();
	if(json==NULL){
		printf("failed to create jsonObject");
		return NULL;
	}

	char *datastr = readMpinfoToJson(data->data,data->dataSize);
	cJSON *dataJson = cJSON_Parse(datastr);

	cJSON_AddItemToObject(json,"data",dataJson);//由于dataJson被添加到json，所以参数json的时候会删除它
	cJSON_AddNumberToObject(json,"dataSize",data->dataSize);
	
	free(datastr);
	char* resStr = cJSON_Print(json);
	cJSON_Delete(json);
	return resStr;
}*/

int jsonToReadMpinfo(char *jsonStr,readMpinfo **data){
	if(jsonStr==NULL || strcmp(jsonStr,"")==0){
		*data=NULL;
		return 0;
	}
	cJSON *jsonArray = cJSON_Parse(jsonStr);
	if(jsonArray==NULL){
		printf("failed to create cJSON\n");
		return -1;
	}
	if(!cJSON_IsArray(jsonArray)) {
        cJSON_Delete(jsonArray);
        return -1;
    }
	size_t arraySize = cJSON_GetArraySize(jsonArray);
	if(arraySize<=0){
		*data=NULL;
		cJSON_Delete(jsonArray);
		return 0;
	}
	
	*data = (readMpinfo*)malloc(sizeof(readMpinfo)*arraySize);
	if(*data == NULL){
		LOG_ERROR("Memory allocation failed");
		cJSON_Delete(jsonArray);
		return -1;
	}

	for(int i=0;i<arraySize;i++){
		cJSON *json = cJSON_GetArrayItem(jsonArray,i);
		if (!cJSON_IsObject(json)) {
            cJSON_Delete(jsonArray);
            return -1;
        }
		cJSON *devid = cJSON_GetObjectItemCaseSensitive(json, "devid");
		if (cJSON_IsNumber(devid)) {
			(*data)[i].devid = devid->valueint;
		}
		cJSON *mpid = cJSON_GetObjectItemCaseSensitive(json, "mpid");
		if (cJSON_IsNumber(mpid)) {
			(*data)[i].mpid = mpid->valueint;
		}
		cJSON *devname = cJSON_GetObjectItemCaseSensitive(json, "devname");
		if (cJSON_IsString(devname)&&(devname->valuestring!=NULL)) {
			strcpy((*data)[i].devname,devname->valuestring);
		}
		
		
		cJSON *mpname = cJSON_GetObjectItemCaseSensitive(json, "mpname");
		if(cJSON_IsString(mpname)&&(mpname->valuestring!=NULL)){
			strcpy((*data)[i].mpname,mpname->valuestring);
		}
		cJSON *groupname = cJSON_GetObjectItemCaseSensitive(json, "groupname");
		if(cJSON_IsString(groupname)&&(groupname->valuestring!=NULL)){
			strcpy((*data)[i].groupname,groupname->valuestring);
		}
		cJSON *address = cJSON_GetObjectItemCaseSensitive(json, "address");
		if(cJSON_IsString(address)&&(address->valuestring!=NULL)){
			strcpy((*data)[i].address,address->valuestring);
		}
		cJSON *mpnote = cJSON_GetObjectItemCaseSensitive(json, "mpnote");
		if(cJSON_IsString(mpnote)&&(mpnote->valuestring!=NULL)){
			strcpy((*data)[i].mpnote,mpnote->valuestring);
		}
		cJSON *value = cJSON_GetObjectItemCaseSensitive(json, "value");
		if(cJSON_IsString(value)&&(value->valuestring!=NULL)){
			strcpy((*data)[i].value,value->valuestring);
		}
		cJSON *time = cJSON_GetObjectItemCaseSensitive(json, "time");
		if(cJSON_IsString(time)&&(time->valuestring!=NULL)){
			strcpy((*data)[i].time,time->valuestring);
		}
		cJSON *ip = cJSON_GetObjectItemCaseSensitive(json, "ip");
		if(cJSON_IsString(ip)&&(ip->valuestring!=NULL)){
			strcpy((*data)[i].ip,ip->valuestring);
		}
		
		cJSON *valuetype = cJSON_GetObjectItemCaseSensitive(json, "valuetype");
		if (cJSON_IsNumber(valuetype)) {
			(*data)[i].valuetype = valuetype->valueint;
		}
		
	}
	
	cJSON_Delete(jsonArray);
	return arraySize;
}

char* readMpinfoToJson(readMpinfo *data,int size){
	cJSON *jsonArray = cJSON_CreateArray();
	if (jsonArray == NULL) {
		printf("failed to create cJSON_CreateArray\n");
        return NULL;
    }
	for(int i=0;i<size;i++){
		cJSON  *json = cJSON_CreateObject();
		if(json == NULL){
			printf("failed to create cJSON_CreateObject\n");
			cJSON_Delete(jsonArray);
			return NULL;
		}
		cJSON_AddNumberToObject(json,"devid",data[i].devid);
		cJSON_AddNumberToObject(json,"mpid",data[i].mpid);
		cJSON_AddStringToObject(json,"devname",data[i].devname);
		cJSON_AddStringToObject(json,"mpname",data[i].mpname);
		cJSON_AddStringToObject(json,"groupname",data[i].groupname);
		cJSON_AddStringToObject(json,"address",data[i].address);
		cJSON_AddStringToObject(json,"mpnote",data[i].mpnote);
		cJSON_AddStringToObject(json,"value",data[i].value);
		cJSON_AddStringToObject(json,"ip",data[i].ip);
		cJSON_AddStringToObject(json,"time",data[i].time);
		cJSON_AddNumberToObject(json,"valuetype",data[i].valuetype);
		
		cJSON_AddItemToArray(jsonArray, json);
	}
	
	char *jsonString = cJSON_Print(jsonArray);
	// 释放JSON对象
	char *resJson = malloc(strlen(jsonString)+1);
	strcpy(resJson,jsonString);
	free(jsonString);
	cJSON_Delete(jsonArray);
	return resJson;
}

char* pollinfoToJson(pollinfo *data,int size){
	cJSON *jsonArray = cJSON_CreateArray();
	if (jsonArray == NULL) {
		printf("failed to create cJSON_CreateArray\n");
        return NULL;
    }
	for(int i=0;i<size;i++){
		cJSON  *json = cJSON_CreateObject();
		if(json == NULL){
			printf("failed to create cJSON_CreateObject\n");
			cJSON_Delete(jsonArray);
			return NULL;
		}
		cJSON_AddNumberToObject(json,"devid",data[i].devid);
		cJSON_AddStringToObject(json,"devname",data[i].devname);
		cJSON_AddStringToObject(json,"ip",data[i].ip);
		cJSON_AddNumberToObject(json,"pollcycle",data[i].pollcycle);
		cJSON_AddNumberToObject(json,"ismultipoll",data[i].ismultipoll);
		cJSON_AddNumberToObject(json,"pollcycle2",data[i].pollcycle2);
		cJSON_AddNumberToObject(json,"istimeout",data[i].istimeout);
		cJSON_AddNumberToObject(json,"collecttime",data[i].collecttime);
		cJSON_AddNumberToObject(json,"connecttime",data[i].connecttime);
		
		cJSON_AddStringToObject(json,"groupname",data[i].groupname);
		cJSON_AddNumberToObject(json,"uploadperiod",data[i].uploadperiod);
		cJSON_AddNumberToObject(json,"maxitem",data[i].maxitem);
		
		cJSON_AddNumberToObject(json,"mpid",data[i].mpid);
		cJSON_AddStringToObject(json,"mpname",data[i].mpname);
		cJSON_AddStringToObject(json,"addresstype",data[i].addresstype);
		cJSON_AddNumberToObject(json,"dbindex",data[i].dbindex);
		cJSON_AddStringToObject(json,"address",data[i].address);
		cJSON_AddNumberToObject(json,"valuetype",data[i].valuetype);
		cJSON_AddStringToObject(json,"mpnote",data[i].mpnote);
		
		cJSON_AddStringToObject(json,"warnname",data[i].warnname);
		cJSON_AddStringToObject(json,"warnlevel",data[i].warnlevel);
		cJSON_AddNumberToObject(json,"warncondition",data[i].warncondition);
		cJSON_AddNumberToObject(json,"wcvalue1",data[i].wcvalue1);
		cJSON_AddNumberToObject(json,"wcvalue2",data[i].wcvalue2);
		cJSON_AddStringToObject(json,"warncontext",data[i].warncontext);
		
		cJSON_AddItemToArray(jsonArray, json);
	}

	char *jsonString = cJSON_Print(jsonArray);
    // 释放JSON对象
    cJSON_Delete(jsonArray);
	return jsonString;
}

int jsonToPollinfo(char *jsonStr,pollinfo **data){
	if(jsonStr==NULL || strcmp(jsonStr,"")==0){
		*data=NULL;
		return 0;
	}
	cJSON *jsonArray = cJSON_Parse(jsonStr);
	if(jsonArray==NULL){
		printf("failed to create cJSON\n");
		return -1;
	}
	if(!cJSON_IsArray(jsonArray)) {
        cJSON_Delete(jsonArray);
        return -1;
    }
	size_t arraySize = cJSON_GetArraySize(jsonArray);
	if(arraySize<=0){
		*data=NULL;
		cJSON_Delete(jsonArray);
		return 0;
	}
	
	*data = malloc(sizeof(pollinfo)*arraySize);
	if(*data == NULL){
		fprintf(stderr, "Memory allocation failed\n");
		cJSON_Delete(jsonArray);
		return -1;
	}
	for(int i=0;i<arraySize;i++){
		cJSON *json = cJSON_GetArrayItem(jsonArray,i);
		if (!cJSON_IsObject(json)) {
            cJSON_Delete(jsonArray);
            return -1;
        }

		cJSON *devid = cJSON_GetObjectItemCaseSensitive(json, "devid");
		if (cJSON_IsNumber(devid)) {
			(*data)[i].devid = devid->valueint;
		}
		cJSON *devname = cJSON_GetObjectItemCaseSensitive(json, "devname");
		if(cJSON_IsString(devname)&&(devname->valuestring!=NULL)){
			strcpy((*data)[i].devname,devname->valuestring);
		}
		cJSON *ip = cJSON_GetObjectItemCaseSensitive(json, "ip");
		if(cJSON_IsString(ip)&&(ip->valuestring!=NULL)){
			strcpy((*data)[i].ip,ip->valuestring);
		}
		cJSON *pollcycle = cJSON_GetObjectItemCaseSensitive(json, "pollcycle");
		if (cJSON_IsNumber(pollcycle)) {
			(*data)[i].pollcycle = pollcycle->valueint;
		}
		cJSON *ismultipoll = cJSON_GetObjectItemCaseSensitive(json, "ismultipoll");
		if (cJSON_IsNumber(ismultipoll)) {
			(*data)[i].ismultipoll = ismultipoll->valueint;
		}
		cJSON *pollcycle2 = cJSON_GetObjectItemCaseSensitive(json, "pollcycle2");
		if (cJSON_IsNumber(pollcycle2)) {
			(*data)[i].pollcycle2 = pollcycle2->valueint;
		}
		cJSON *istimeout = cJSON_GetObjectItemCaseSensitive(json, "istimeout");
		if (cJSON_IsNumber(istimeout)) {
			(*data)[i].istimeout = istimeout->valueint;
		}
		cJSON *collecttime = cJSON_GetObjectItemCaseSensitive(json, "collecttime");
		if (cJSON_IsNumber(collecttime)) {
			(*data)[i].collecttime = collecttime->valueint;
		}
		cJSON *connecttime = cJSON_GetObjectItemCaseSensitive(json, "connecttime");
		if (cJSON_IsNumber(connecttime)) {
			(*data)[i].connecttime = connecttime->valueint;
		}
		
		cJSON *groupname = cJSON_GetObjectItemCaseSensitive(json, "groupname");
		if(cJSON_IsString(groupname)&&(groupname->valuestring!=NULL)){
			strcpy((*data)[i].groupname,groupname->valuestring);
		}
		cJSON *uploadperiod = cJSON_GetObjectItemCaseSensitive(json, "uploadperiod");
		if (cJSON_IsNumber(uploadperiod)) {
			(*data)[i].uploadperiod = uploadperiod->valueint;
		}
		cJSON *maxitem = cJSON_GetObjectItemCaseSensitive(json, "maxitem");
		if (cJSON_IsNumber(maxitem)) {
			(*data)[i].maxitem = maxitem->valueint;
		}
		
		cJSON *mpid = cJSON_GetObjectItemCaseSensitive(json, "mpid");
		if (cJSON_IsNumber(mpid)) {
			(*data)[i].mpid = mpid->valueint;
		}	
		cJSON *mpname = cJSON_GetObjectItemCaseSensitive(json, "mpname");
		if(cJSON_IsString(mpname)&&(mpname->valuestring!=NULL)){
			strcpy((*data)[i].mpname,mpname->valuestring);
		}
		cJSON *addresstype = cJSON_GetObjectItemCaseSensitive(json, "addresstype");
		if(cJSON_IsString(addresstype)&&(addresstype->valuestring!=NULL)){
			strcpy((*data)[i].addresstype,addresstype->valuestring);
		}
		cJSON *dbindex = cJSON_GetObjectItemCaseSensitive(json, "dbindex");
		if (cJSON_IsNumber(dbindex)) {
			(*data)[i].dbindex = dbindex->valueint;
		}	
		cJSON *address = cJSON_GetObjectItemCaseSensitive(json, "address");
		if(cJSON_IsString(address)&&(address->valuestring!=NULL)){
			strcpy((*data)[i].address,address->valuestring);
		}
		cJSON *valuetype = cJSON_GetObjectItemCaseSensitive(json, "valuetype");
		if (cJSON_IsNumber(valuetype)) {
			(*data)[i].valuetype = valuetype->valueint;
		}		
		cJSON *mpnote = cJSON_GetObjectItemCaseSensitive(json, "mpnote");
		if(cJSON_IsString(mpnote)&&(mpnote->valuestring!=NULL)){
			strcpy((*data)[i].mpnote,mpnote->valuestring);
		}
		
		cJSON *warnname = cJSON_GetObjectItemCaseSensitive(json, "warnname");
		if(cJSON_IsString(warnname)&&(warnname->valuestring!=NULL)){
			strcpy((*data)[i].warnname,warnname->valuestring);
		}
		cJSON *warnlevel = cJSON_GetObjectItemCaseSensitive(json, "warnlevel");
		if(cJSON_IsString(warnlevel)&&(warnlevel->valuestring!=NULL)){
			strcpy((*data)[i].warnlevel,warnlevel->valuestring);
		}
		cJSON *warncondition = cJSON_GetObjectItemCaseSensitive(json, "warncondition");
		if (cJSON_IsNumber(warncondition)) {
			(*data)[i].warncondition = warncondition->valueint;
		}	
		cJSON *wcvalue1 = cJSON_GetObjectItemCaseSensitive(json, "wcvalue1");
		if (cJSON_IsNumber(wcvalue1)) {
			(*data)[i].wcvalue1 = (float)wcvalue1->valuedouble;
		}	
		cJSON *wcvalue2 = cJSON_GetObjectItemCaseSensitive(json, "wcvalue2");
		if (cJSON_IsNumber(wcvalue2)) {
			(*data)[i].wcvalue2 = (float)wcvalue2->valuedouble;
		}	
		cJSON *warncontext = cJSON_GetObjectItemCaseSensitive(json, "warncontext");
		if(cJSON_IsString(warncontext)&&(warncontext->valuestring!=NULL)){
			strcpy((*data)[i].warncontext,warncontext->valuestring);
		}	
	}

	cJSON_Delete(jsonArray);
	return arraySize;
}

int jsonToParam(char *jsonStr, paraData **data){
	printf("debug 4.1.1\n");
	if(jsonStr==NULL || strcmp(jsonStr,"")==0){
		return -1;
	}
	cJSON *json = cJSON_Parse(jsonStr);
	if(*data==NULL){
		*data = paramInit(NULL,0,0);
	}
	//*data = (paraData*)malloc(sizeof(paraData));
	cJSON *msg = cJSON_GetObjectItemCaseSensitive(json,"msg");
	printf("debug 4.1.2\n");
	if(cJSON_IsString(msg)){
		printf("debug 4.1.2.0\n");
		char *tmp = cJSON_Print(msg);
		printf("debug 4.1.2.1\n");
		(*data)->msg = (char*)malloc(sizeof(char)*strlen(tmp) + 2);
		printf("debug 4.1.2.2\n");
		free(tmp);
		printf("debug 4.1.2.3\n");
		strcpy((*data)->msg,msg->valuestring);
	}
	printf("debug 4.1.3\n");
	cJSON *dataArrySize = cJSON_GetObjectItemCaseSensitive(json,"dataArrySize");
	if(cJSON_IsNumber(dataArrySize)){
		(*data)->dataArrySize = dataArrySize->valueint;
	}
	cJSON *code = cJSON_GetObjectItemCaseSensitive(json,"code");
	if(cJSON_IsNumber(code)){
		(*data)->code = code->valueint;
	}	
	printf("debug 4.1.4\n");
	cJSON *dataArray = cJSON_GetObjectItemCaseSensitive(json,"dataArray");
	char *data_string = cJSON_Print(dataArray);
	printf("data_string : %s\n",data_string);
	int mapLen = sizeof(functionMap)/sizeof(functionMap[0]),arraySize=0;
	if(data_string != NULL){
		for(int i=0;i<mapLen;i++){//有发送的是有结构体
			if(strcmp(functionMap[i].key,(*data)->msg)==0){
				printf("key = %s   msg=%s\n",functionMap[i].key,(*data)->msg);
				arraySize = functionMap[i].toStruct(data_string,(void**)&((*data)->dataArray));
				break;
			}
		}
		if(arraySize==0){//若arraySize=0说明没有找到对应的json转结构体的函数，则dataArray可能为NULL或者一个字符串
			if(strcmp(data_string,"")!=0  &&  strcmp(data_string,"[]")!=0){//因为dataArray初始化为NULL，所以这里只考虑data_string为字符串的情况
				int tmpLen = strlen(data_string)+1;
				(*data)->dataArray = (char*)malloc(sizeof(char)*tmpLen);
				strncpy((*data)->dataArray,data_string,tmpLen);
				arraySize = 1;
			}
		}
		(*data)->dataArrySize = arraySize;
		free(data_string);
	}
	
	cJSON_Delete(json);
	printf("json to param over!\n");
	return arraySize;
}

char* paramToJson(paraData *data){
	cJSON *json = cJSON_CreateObject();
	if(data->msg==NULL)
		cJSON_AddStringToObject(json,"msg","");
	else
		cJSON_AddStringToObject(json,"msg",data->msg);

	cJSON_AddNumberToObject(json,"code",data->code);
	cJSON_AddNumberToObject(json,"dataArrySize",data->dataArrySize);
	
	//dataArray有三种情况，NULL，字符串，一个有对应json转结构体函数得值
	int mapLen = sizeof(functionMap)/sizeof(functionMap[0]);
	char isStrFlag = 1;
	for(int i=0;i<mapLen;i++){//返回的是一个结构体  有对应json转结构体函数得值
		if(data->msg==NULL)
			break;
		if(strcmp(functionMap[i].key,data->msg)==0){
			char *tmpStr = functionMap[i].toJson((void*)data->dataArray,data->dataArrySize);
			cJSON *dataArray = cJSON_Parse(tmpStr);
			cJSON_AddItemToObject(json,"dataArray",dataArray);
			free(tmpStr);
			isStrFlag = 0;
			break;
		}
		
	}
	if(data->dataArray==NULL){//返回的值为NULL
		cJSON_AddStringToObject(json,"dataArray","");
	}else if(isStrFlag){//返回的是一个字符串
	
		char *tmpStr = strToJson((void*)data->dataArray,data->dataArrySize);
//		printf("tmpStr:%s\n",tmpStr);
		//cJSON *dataArray = cJSON_Parse(tmpStr);
		cJSON_AddStringToObject(json,"dataArray",tmpStr);
		//free(tmpStr);//当data->dataArray时字符串的时候，tmpStr指向data->dataArray，如果这时执行了free(tmpStr)，那data->dataArray将被free，再执行paramFree会报错double free
	}
	char *resStr = cJSON_Print(json);
//	printf("to json:%s\n",resStr);
	cJSON_Delete(json);
	return resStr;
}

void paramFree(paraData *data){
	if(data->dataArray != NULL)
		free(data->dataArray);
	if(data->msg != NULL)
		free(data->msg);
	if(data != NULL)
		free(data);
	data = NULL;
}

paraData* paramInit(char *topic,int dataArrySize,int code){
	paraData *data = (paraData*)malloc(sizeof(paraData));
	data->dataArray = NULL;
	
	if(topic!=NULL){
		data->msg = (char*)malloc( (strlen(topic)+1)*sizeof(char) );
		strcpy(data->msg,topic);
	}else
		data->msg = NULL;
	
	data->code = code;
	data->dataArrySize = dataArrySize;
	return data;
}

int jsonTostr(char *jsonStr,char **str){//这里是将字符直接赋值给paraData的dataArray，
	*str = (char*)malloc((strlen(jsonStr)+1)*sizeof(char));
	strcpy(*str,jsonStr);
	return 1;
}

char *strToJson(char* str,int size){
	return str;
}

/*
int main(){
	pollinfo info[2] = {
					{"plc1","192.168.3.248",2,0,0,0,5000,5000,\
						500,500,"mp1","DB",1,"vx20",1,"warn1",\
						"提醒",406,23.24,32.50,"this is a  text"},\
					{"plc2","192.168.3.248",2,0,0,0,5000,5000,\
						500,500,"mp1","DB",1,"vx20",1,"warn1",\
						"提醒",406,23.24,32.50,"this is a  text"}
					};
					
	char *res = structToJson(info,sizeof(info)/sizeof(info[0]));
	printf("%s\n",res);
	pollinfo *info2 = NULL;
	int size = jsonToStruct(res,&info2);
	for(int i=0;i<size;i++){
		printf("devname1:%s\n",info2[i].devname);
	}
	pollinfo *info3 = info2;
	printf("devname3:%s\n",info3[1].devname);
	free(info2);
		

			
	char *res = structToJson(&info);
	printf("%s\n",res);
	pollinfo info2;
	jsonToStruct(res,&info2);
	printf("devname:%s\n",info2.devname);
	printf("ip:%s\n",info2.ip);
	printf("pollcycle:%d\n",info2.pollcycle);
	printf("ismultipoll:%d\n",info2.ismultipoll);
	printf("pollcycle2:%d\n",info2.pollcycle2);
	printf("istimeout:%d\n",info2.istimeout);
	printf("collecttime:%d\n",info2.collecttime);
	printf("connecttime:%d\n",info2.connecttime);
	
	printf("uploadperiod:%d\n",info2.uploadperiod);
	printf("maxitem:%d\n",info2.maxitem);
	
	printf("mpname:%s\n",info2.mpname);
	printf("addresstype:%s\n",info2.addresstype);
	printf("dbindex:%d\n",info2.dbindex);
	printf("address:%s\n",info2.address);
	printf("valuetype:%d\n",info2.valuetype);
	
	printf("warnname:%s\n",info2.warnname);
	printf("warnlevel:%s\n",info2.warnlevel);
	printf("warncondition:%d\n",info2.warncondition);
	printf("wcvalue1:%f\n",info2.wcvalue1);
	printf("wcvalue2:%f\n",info2.wcvalue2);
	printf("warncontext:%s\n",info2.warncontext);
	return 0;
}*/
