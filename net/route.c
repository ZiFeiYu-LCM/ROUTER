#include "route.h"

#define ALL 0
#define STATIC 1
void* routeShow(void *param){
	//routerFlag=1
	char *arg = (char*)param;
	strtok(arg,"=");
	char flag = atoi(strtok(NULL,"="));
	
	char substr[100];
	FILE *fp;
	char line[MAX_ROUTE_INFO];


	if(flag==ALLROUTER){
		fp = popen("ip route show","r");
	}else if(flag==STATICROUTER){
		fp = popen("ip route show | grep \"scope link\"","r");
	}else if(flag==LINKROUTER){
		fp = popen("ip route show | grep -vE \"proto (dhcp|zebra|bgp|rip|ospf)\"","r");
	}
	else if(flag==RIPROUTER){
		fp = popen("ip route show | grep \"proto rip\"","r");
	}
	else if(flag==OSPFROUTER){
		fp = popen("ip route show | grep \"proto ospf\"","r");
	}
	else if(flag==BGPROUTER){
		fp = popen("ip route show | grep \"proto bgp\"","r");
	}
	//fp = popen("ip route show","r");	

	int itemNum = 0;
	
	FILE *tmpFile = fopen(ROUTER_VAR_PATH,"w"); 
	if(tmpFile==NULL || fp==NULL){
		LOG_ERROR("route: popen or fopen");
		return REQUEST_ERROR(100,"route: popen or fopen");	
	}
	while(fgets(line, MAX_ROUTE_INFO, fp)){
		fputs(line,tmpFile);
		memset(line,0,MAX_ROUTE_INFO);
		itemNum++;
	}
	fseek(fp, 0, SEEK_SET);
	routeInfo *resStr = (routeInfo*)malloc(sizeof(routeInfo)*itemNum);
	while(fgets(line, MAX_ROUTE_INFO, tmpFile)){
		
		
		
		
		
		memset(line,0,MAX_ROUTE_INFO);
	}
	
	
	fclose(tmpFile);
	
	pclose(fp);
	return NULL;	
}

void* hostShow(){
	char *topic = "router/showhost";
	char *res;
	char line[MAX_ROUTE_INFO],iterface[10];
	FILE *fp = NULL;
	
	system("ip -d -a");//清除缓存
	//fp = popen("ip n","r");	//   cat /var/lib/misc/dnsmasq.leases  查看dnsmasq的租约
	fp = fopen(DHCP_CONF_PATH,"r");
	if(fp==NULL){
		perror("failed to fopen");
		return "error:failed to fopen DHCP_CONF_PATH";	
	}
	while(fgets(line, MAX_ROUTE_INFO, fp)){
		if(strstr(line,"interface=")){
			char *tmp = strtok(line,"=");
			tmp = strtok(NULL,"=");
			strncpy(iterface,tmp,10);
			memset(line,0,MAX_ROUTE_INFO);
			break;
		}
		memset(line,0,MAX_ROUTE_INFO);
	}
	fclose(fp);
	fp = NULL;
	
	fp = popen("cat /var/lib/misc/dnsmasq.leases","r");
	if(fp==NULL){
		perror("failed to popen");
		return "error:failed to popen dnsmasq.leases";	
	}
	
	int number = 0;
	hostinfo *hostinfoArray = NULL; (hostinfo*)malloc(sizeof(hostinfo)*number);
	
	while(fgets(line, MAX_ROUTE_INFO, fp)){
		number++;
		hostinfo *tmp = (hostinfo*)realloc(hostinfoArray,sizeof(hostinfo)*number);
		if(tmp==NULL){
			if(hostinfoArray!=NULL)
				free(hostinfoArray);
			return "error:failed to realloc memory";
		}
		hostinfoArray = tmp;
		char *tmpstr = strtok(line, " ");
		printf("%s\n",tmp);
		strncpy(hostinfoArray[number-1].validtime,tmpstr,20);
		
		tmp = strtok(NULL, " ");
		printf("%s\n",tmp);
		strncpy(hostinfoArray[number-1].mac,tmpstr,50);
		
		tmp = strtok(NULL, " ");
		printf("%s\n",tmp);
		strncpy(hostinfoArray[number-1].ip,tmpstr,20);
		
		tmp = strtok(NULL, " ");
		printf("%s\n",tmp);
		strncpy(hostinfoArray[number-1].name,tmpstr,30);
		
		strncpy(hostinfoArray[number-1].interface,iterface,10);
		//if(strstr(line,"REACHABLE"))
		//	printf("%s",line);
		memset(line,0,MAX_ROUTE_INFO);
	}
	pclose(fp);
	
	paraData *data = paramInit(topic,number,200);
	data->dataArray = hostinfoArray;
	res = paramToJson(data);
	paramFree(data);
	return res;	
}


void* staticRouteShow(){
	char line[MAX_ROUTE_INFO];
	FILE *fp;
	fp = popen("ip route show","r");	
	if(fp==NULL){
		perror("failed to popen");
		return NULL;	
	}
	while(fgets(line, MAX_ROUTE_INFO, fp)){
		if(strstr(line,"proto static") || (!strstr(line,"broadcast") && !strstr(line,"scope link") && !strstr(line,"dhcp") && strstr(line,"metric")) ){
			
		}
		printf("%s",line);
		memset(line,0,MAX_ROUTE_INFO);
	}
	pclose(fp);
	return NULL;	
}

void* addStaticRoute(void* data){
//ip route add 192.168.10.0/16 via 192.168.10.1 dev eth0 metric 253 track qwer
	routeInfo *info = (routeInfo*)data;
	if(!(info->ip && info->netmask)){
		printf("ip or netmask is NULL\n");
		return NULL;	
	}
	FILE *fp;
	int netmask = 24;
	char line[MAX_ROUTE_INFO];
	if(strcmp(info->netmask,"255.255.255.255")==0)
		netmask=32;
	else if(strcmp(info->netmask,"255.255.0.0")==0)
		netmask=16;
	else if(strcmp(info->netmask,"255.0.0.0")==0)
		netmask=8;
	else if(strcmp(info->netmask,"0.0.0.0")==0)
		netmask=0;
	char cmd[1024];
	sprintf(cmd,"ip route add %s/%d",info->ip,netmask);
	if(info->gateway)
		sprintf(cmd,"%s via %s",cmd,info->gateway);
	if(info->interface)
		sprintf(cmd,"%s dev %s",cmd,info->interface);
	if(info->metric)
		sprintf(cmd,"%s metric %s",cmd,info->metric);
	
	printf("%s\n",cmd);
	fp = popen(cmd,"r");	
	if(fp==NULL){
		perror("failed to popen");
		return NULL;	
	}

	while(fgets(line, MAX_ROUTE_INFO, fp)){
		printf("%s",line);
		memset(line,0,MAX_ROUTE_INFO);
	}
	pclose(fp);
	return NULL;	
}





















