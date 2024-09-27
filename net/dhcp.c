#include "dhcp.h"

char udhcpdFlag = 0;//用来主程序监控该程序的启用状态

/*
char* setDnsDomain(void *arg){
	paraData *data = NULL;
	jsonToParam(arg,&data);
	if(data==NULL){
		return "error:dns param error";
	}
	dnsinfo *infoArray = data->dataArray;
	dnsinfo *info = &infoArray[0];
	
	dhcpdstop();
	
	
    FILE* new_fp=NULL;
    FILE* old_fp=NULL;
    char cmd[200];
    snprintf(cmd,200,"cp %s %s",DHCP_CONF_PATH,OLD_CONFIG_PATH(DHCP_CONF_PATH));
    system(cmd);


    new_fp = fopen(DHCP_CONF_PATH,"w");
    old_fp = fopen(OLD_CONFIG_PATH(DHCP_CONF_PATH),"r");
    if(old_fp==NULL || new_fp==NULL){
        startssh(NULL);
        return "error:failed to open dns.conf";
    }
	char line[1024];
	char falg1 = 0;//用来表示是否都已成功改写line
	char falg2 = 0;
    while(fgets(line,1024,old_fp)){
		if((falg1+falg2)<2){
			if(!falg1 && strstr(line,"server=")){
				if(info->domain1)
					snprintf(line,1024,"server=%s",info->domain1);
				else
					snprintf(line,1024,"#server=",NULL);
				falg1=1;
			}
			if(!falg2 && strstr(line,"server=")){
				if(info->domain2)
					snprintf(line,1024,"server=%s",info->domain2);
				else
					snprintf(line,1024,"#server=",NULL);
				falg2=1;
			}
		}
		
		fputs(line,new_fp);
        memset(line,0,1024);
	}
	if(!falg1){
		snprintf(line,1024,"server=%s",info->domain1);
		fputs(line,new_fp);
	}
	if(!falg2){
		snprintf(line,1024,"server=%s",info->domain2);
		fputs(line,new_fp);
	}
	fclose(new_fp);
	fclose(old_fp);
	delFile(OLD_CONFIG_PATH(DHCP_CONF_PATH));
	dhcpdstart();
	return NULL;
}

char* getRelay(void *arg){
	char *topic = "dhcp/getrelay";
	int num=0;
	
	FILE* old_fp=NULL;
	old_fp = fopen(DHCP_CONF_PATH,"r");
	if(old_fp==NULL){
        startssh(NULL);
        return "error:failed to open dns.conf";
    }
	char line[1024];
	while(fgets(line,1024,old_fp)){
		if(strstr(line,"address=")){
			num++;
		}
		memset(line,0,1024);
	}
	fclose(old_fp);
	
	dnsinfo *dnsinfoList = (dnsinfo*)malloc(sizeof(dnsinfo)*num);
	paraData *data = paramInit(topic,num,200);
	
	old_fp = fopen(DHCP_CONF_PATH,"r");
	if(old_fp==NULL){
        startssh(NULL);
        return "error:failed to open dns.conf";
    }
	num = 0;
	while(fgets(line,1024,old_fp)){
		if(strstr(line,"address=")){
			strtok(line,'/');
			char *str = strtok(NULL,'/');
			strncpy(dnsinfoList[num].domain1,sizeof(str), str);
			str = strtok(NULL,'/');
			strncpy(dnsinfoList[num].ip1,sizeof(str), str);
			num++;
		}
		memset(line,0,1024);
	}
	fclose(old_fp);
	
	data->dataArray = dnsinfoList;
	char *resJson = paramToJson(data);//返回数组json串
	
	paramFree(data);
	return resJson;
}


char* delRelay(void *arg){
	paraData *data = NULL;
	jsonToParam(arg,&data);
	if(data==NULL){
		return "error:dns param error";
	}
	dnsinfo *infoArray = data->dataArray;
	dnsinfo *info = &infoArray[0];
	
	dhcpdstop();
	
	FILE* new_fp=NULL;
    FILE* old_fp=NULL;
    char cmd[200];
    snprintf(cmd,200,"mv %s %s",DHCP_CONF_PATH,OLD_CONFIG_PATH(DHCP_CONF_PATH));
    system(cmd);
    snprintf(cmd,200,"touch %s",DHCP_CONF_PATH);
    system(cmd);

    new_fp = fopen(DHCP_CONF_PATH,"w");
    old_fp = fopen(OLD_CONFIG_PATH(DHCP_CONF_PATH),"r");
    if(old_fp==NULL || new_fp==NULL){
        startssh(NULL);
        return "error:failed to open dns.conf";
    }
	char line[1024];
	while(fgets(line,1024,old_fp)){
		if(strstr(line,info->domain1)){
			memset(line,0,1024);
			continue;
		}
		fputs(line,new_fp);
		memset(line,0,1024);
	}
	fclose(new_fp);
	fclose(old_fp);
	delFile(OLD_CONFIG_PATH(DHCP_CONF_PATH));
	dhcpdstart();
	
	return NULL;	
}

char* setDnsRelay(void *arg){
	paraData *data = NULL;
	jsonToParam(arg,&data);
	if(data==NULL){
		return "error:dns param error";
	}
	dnsinfo *infoArray = data->dataArray;
	dnsinfo *info = &infoArray[0];
	
	dhcpdstop();
	
    FILE* new_fp=NULL;
    char cmd[200];
    snprintf(cmd,200,"cp %s %s",DHCP_CONF_PATH,OLD_CONFIG_PATH(DHCP_CONF_PATH));
    system(cmd);


    new_fp = fopen(DHCP_CONF_PATH,"a");
	if(new_fp==NULL){
		return "error:failed to open config";
	}
	snprintf(cmd,200,"echo /%s/%s >> %s",info->domain1,info->ip1,DHCP_CONF_PATH);
	cmdRun(cmd);
	if(info->ip2){
		snprintf(cmd,200,"echo /%s/%s >> %s",info->domain1,info->ip2,DHCP_CONF_PATH);
		cmdRun(cmd);
	}

	fclose(new_fp);
	snprintf(cmd,200,"rm %s",OLD_CONFIG_PATH(DHCP_CONF_PATH));
    system(cmd);
	
	dhcpdstart();
	
	
	
	return NULL;	
}


char* relaystart(void *arg){
	dhcpdstop();
    FILE* old_fp=NULL;
    old_fp = fopen(DHCP_CONF_PATH,"r+");
    if(old_fp==NULL){
        return "error:failed to open dns.conf";
    }
	char line[1024];
	char flag = 0;//用来检测是否有no-resolv
	while(fgets(line,1024,old_fp)){
		if(strstr(line,"no-resolv")){
			memset(line,0,1024);
			flag = 1;
			break;
		}
		memset(line,0,1024);
	}
	if(!flag){
		snprintf(line,1024,"no-resolv",NULL);
		fputs(line,old_fp);
	}
	fclose(old_fp);
	dhcpdstart();
	return NULL;
}

char* relaystop(void *arg){
	dhcpdstop();
	FILE* new_fp=NULL;
    FILE* old_fp=NULL;
    char cmd[200];
    snprintf(cmd,200,"mv %s %s",DHCP_CONF_PATH,OLD_CONFIG_PATH(DHCP_CONF_PATH));
    system(cmd);
    snprintf(cmd,200,"touch %s",DHCP_CONF_PATH);
    system(cmd);

    new_fp = fopen(DHCP_CONF_PATH,"w");
    old_fp = fopen(DHCP_CONF_PATH,"r");
    if(old_fp==NULL || new_fp==NULL){
        startssh(NULL);
        return "error:failed to open dns.conf";
    }
	char line[1024];
	while(fgets(line,1024,old_fp)){
		if(strstr(line,"no-resolv")){
			memset(line,0,1024);
			continue;
		}
		
		fputs(line,new_fp);
		memset(line,0,1024);
	}
	
	fclose(new_fp);
	fclose(old_fp);
	delFile(OLD_CONFIG_PATH(DHCP_CONF_PATH));
	dhcpdstart();
	return NULL;
}

*/

char* getdhcpinfo(void *arg){
	char *res = NULL;
	char *topic = "dhcp/getdhcpinfo";
	FILE *fp = fopen(DHCP_CONF_PATH,"r");
	if(fp==NULL){
		return "error:failed to open dhcp conf file";
	}
	
	dhcpinfo *infoArray =  (dhcpinfo*)malloc(sizeof(dhcpinfo)*1);
	char line[MAXLENGTH];
	while(fgets(line,MAXLENGTH,fp)){
		if(strstr(line,"interface=")){
			sscanf(line,"interface=%s",&infoArray[0].interface);
		}
		if(strstr(line,"dhcp-range=")){
			sscanf(line,"dhcp-range=%[^,],%[^,],%d",&infoArray[0].iprange_start,&infoArray[0].iprange_end,&infoArray[0].validtime);
		}
		memset(line,0,MAXLENGTH);
	}
	fclose(fp);
	
	paraData *data = paramInit(topic,1,200);
	data->dataArray = infoArray;
	res = paramToJson(data);
	paramFree(data);
	return res;
}

char* setiprange(void *arg){
	paraData *data = NULL;
	
	int tmpNum = jsonToParam(arg,&data);
	if(tmpNum<=0){
		paramFree(data);
		return "error:failed to read param";
	}
	
	dhcpinfo *infoArray = data->dataArray;
	dhcpinfo *info = &infoArray[0];
	FILE *fp_new=NULL,*fp_old=NULL;
	char cmd[200];
	snprintf(cmd,200,"cp %s %s",DHCP_CONF_PATH,OLD_CONFIG_PATH(DHCP_CONF_PATH));
	cmdRun(cmd);
	dhcpstop(NULL);
	fp_new = fopen(DHCP_CONF_PATH,"w");
	fp_old = fopen(OLD_CONFIG_PATH(DHCP_CONF_PATH),"r");
	if(fp_new==NULL || fp_old==NULL){
		if(fp_new!=NULL)
			fclose(fp_new);
		else
			fclose(fp_old);
		
		printf("fp_new==NULL \n");
		
		snprintf(cmd,200,"rm %s",DHCP_CONF_PATH);
		cmdRun(cmd);
		
		snprintf(cmd,200,"mv %s %s",OLD_CONFIG_PATH(DHCP_CONF_PATH),DHCP_CONF_PATH);
		cmdRun(cmd);
		
		dhcpstart(NULL);
		return "error:failed to read dhcp config file";
	}
	
	char line[MAXLENGTH];
	while(fgets(line,MAXLENGTH,fp_old)){
		if(strstr(line,"dhcp-range=") && strlen(info->iprange_start)>0 && strlen(info->iprange_end)>0 && info->validtime>0){
			snprintf(line,MAXLENGTH-1,"dhcp-range=%s,%s,%ds\n",info->iprange_start,info->iprange_end,info->validtime);
		}
		if(strstr(line,"interface=") && strlen(info->interface)>0){
			snprintf(line,MAXLENGTH-1,"interface=%s\n",info->interface);
		}
		
		fputs(line,fp_new);
		memset(line,0,MAXLENGTH);
	}
	fclose(fp_new);
	fclose(fp_old);
	dhcpstart(NULL);
	snprintf(cmd,200,"rm %s",OLD_CONFIG_PATH(DHCP_CONF_PATH));
	cmdRun(cmd);
	return NULL;
}

char* dhcpstart(void *arg){
	char cmd[] = "/etc/init.d/dnsmasq start";
	cmdRun(cmd);
	if(is_process_running("dnsmasq")){
		udhcpdFlag = 1;
	}else{
		return "error:failed to start dnsmasq";
	}
	return NULL;
}

char* dhcpstop(void *arg){
	udhcpdFlag = 0;
	char cmd[] = "/etc/init.d/dnsmasq stop";
	cmdRun(cmd);
	return NULL;
}














