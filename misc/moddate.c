#include "moddate.h"

void* setDate(void *arg){
	//格式为"date -s '2022-03-15 10:30:00'"
	char *res = NULL;
	paraData* data = NULL;
	int tmpNum = jsonToParam(arg,&data);
	if(tmpNum==0){
		return "error:failed to read param";
	}
	char *dateStr[100];
	strncpy(dateStr,data->dataArray,100);
	
	char timecmd[50];
	sprintf(timecmd,"date -s %s",(char *)dateStr);
	printf("timecmd 的参数：%s\n",timecmd);
	if(system(timecmd)<0){
		res = (char*)malloc(sizeof(char)*101);
		snprintf(res,100,"error:failed to set time %s",dateStr);
		printf("failed to set time!\n");
	}else{
		printf("set time successful!\n");
	}
	paramFree(data);
	return res;  
}

void* getDate(void *arg){
	//return "error:jsonStr is NULL";
	char *topic = "time/gettime";
	time_t now;
	struct tm *local_time;
	time(&now);
	local_time = localtime(&now);
	printf("current time:%s\n",asctime(local_time));
	
	char tmp[40];
	snprintf(tmp,40,"%s",asctime(local_time));
	char *tmptime = strtok(tmp,"\n");
	
	paraData* data = paramInit(topic,1,0);
//	data->dataArray = asctime(local_time);
	data->dataArray = (char*)malloc(sizeof(char)*40);
	snprintf(data->dataArray,40,"%s",tmptime);
//	strncpy(data->dataArray,asctime(local_time),40);
	char *resStr = paramToJson(data);
	
	paramFree(data);
	return resStr;
}

void* setTimezone(void *arg){
	char *res = NULL;
	paraData* data = NULL;
	int tmpNum = jsonToParam(arg,&data);
	if(tmpNum==0){
		return "error:failed to read param";
	}
	
//    // 设置 TZ 环境变量
//    setenv("TZ", (char*)data->dataArray, 1);
//    // 应用新的时区
//    tzset();
	
	char cmd[100];
	snprintf(cmd,100,"echo \"%s\" > /etc/TZ",(char*)data->dataArray);
	printf("cmd = %s\n",cmd);
	cmdRun(cmd);
	paramFree(data);
	return res;
}


void* getsntpinfo(void *arg){
	char* topic = "time/getsntpinfo";
	FILE* fp = fopen(SNTP_PATH,"r");
	if(fp==NULL){
		return "error:failed to read config file";
	}
	char line[MAXLENGTH];
	ntpserver* serverArray = NULL;
	int num = 0;
	while(fgets(line,MAXLENGTH,fp)){
		if(strncmp(line,"STEP_SERVERS=",strlen("STEP_SERVERS="))==0){
			char servers[MAXLENGTH];
			strtok(line,"=");
			char* ptr = strtok(NULL,"=");
			strcpy(servers,ptr+1);
			servers[strlen(servers)-1]='\0';
			
			
			ptr=strtok(servers," ");
			if(ptr==NULL)
				continue;
			do{
				ntpserver* tmp = (ntpserver*)realloc(serverArray,sizeof(ntpserver)*(num+1));
				if(tmp==NULL){
					if(serverArray!=NULL)
						free(serverArray);
					return "error:failed to init memory";
				}
				serverArray = tmp;
				strcpy(serverArray[num].server,ptr);
//				printf("%s\n",ptr);
				num++;
			}while((ptr=strtok(NULL," "))!=NULL);
		}
		memset(line,0,MAXLENGTH);
	}
	fclose(fp);
	ntpinfo *ntpArray = (ntpinfo*)malloc(sizeof(ntpinfo)*1);
	ntpArray->servers=serverArray;
	ntpArray->serverNum=num;
	
	
	paraData *data=paramInit(topic,1,200);
	data->dataArray=ntpArray;
	char *res = paramToJson(data);
	paramFree(data);
	
	return res;
}

void* setsntpinfo(void *arg){
	
}

void* getNTPinfo(void *arg){
	char *topic = "time/getntpinfo";
	char line[MAXLENGTH];
	FILE *fp = fopen(NTP_PATH,"r");
	if(fp==NULL){
		printf("fp=NULL\n");
		return "error:failed to read config file";
	}
	ntpserver *serverInfoArray = NULL; //= (ntpserver*)malloc(sizeof(ntpserver)*1);
	ntpinfo *infoArray= (ntpinfo*)malloc(sizeof(ntpinfo)*1);
	int num = 0;
	while(fgets(line,1024,fp)){
		if(strncmp(line,"server",strlen("server"))==0){
			ntpserver* tmp = (ntpserver*)realloc(serverInfoArray,sizeof(ntpserver)*(num+1));		
			if(tmp == NULL){
				free(serverInfoArray);
				return "error:memory allocation failed";
			}
			serverInfoArray = tmp;
			
			if(strstr(line,"iburst"))
				serverInfoArray[num].ismain=1;
			else
				serverInfoArray[num].ismain=0;
			
			if(strstr(line,"port") && !strstr(line,"#port")){
				if(strstr(line,"iburst"))
					sscanf(line,"%*s %*s %*s %*s %s",serverInfoArray[num].port);
				else{
					sscanf(line,"%*s %*s %*s %s",serverInfoArray[num].port);
				}
			}else{
				strcpy(serverInfoArray[num].port,"123");
			}
			sscanf(line,"%*s %s",serverInfoArray[num].server);
			num++;		
		}
		if(strncmp(line,"interface listen",strlen("interface listen"))==0){
			if(strlen(line)<=23)
				sscanf(line,"%*s %*s %s ",infoArray->interface);
			else{
				sscanf(line,"%*s %*s %s ",infoArray->srcaddr);
			}
			
		}
		if(strncmp(line,"maxpoll",strlen("maxpoll"))==0){
			sscanf(line,"%*s %d",&(infoArray->maxpoll));
		}		
		memset(line,0,1024);
	}
	fclose(fp);
	infoArray->servers = serverInfoArray;
	infoArray->serverNum = num;

	paraData *data = paramInit(topic,1,200);
	data->dataArray = infoArray;
	
//	printf("serverInfoArray[0]: %s %d\n",serverInfoArray[0].server, serverInfoArray[0].ismain);
//	printf("server[0]: %s\n",((ntpserver*)((ntpinfo*)data->dataArray)[0].servers)[0].server);
	char *res = paramToJson(data); 
	paramFree(data);
//	printf("getNTPinfo res: %s\n",res);
	return res;
}

void* setNTPinfo(void *arg){
	paraData *data = NULL;
	jsonToParam(arg,&data);
	if(data == NULL){
		return "error:failed to read param";
	}
	ntpinfo *infoArray = data->dataArray;
	ntpinfo *info = &infoArray[0];
	FILE *new_fp=NULL,*old_fp=NULL;
	char cmd[100];
	snprintf(cmd,100,"cp %s %s",NTP_PATH,OLD_CONFIG_PATH(NTP_PATH));
	printf("cmd: %s \n",cmd);
	
	cmdRun(cmd);
	stopNTP(NULL);
	
	new_fp = fopen(NTP_PATH,"w");
	old_fp = fopen(OLD_CONFIG_PATH(NTP_PATH),"r");
	if(new_fp==NULL || old_fp==NULL){
		if(new_fp!=NULL)
			fclose(new_fp);
		if(old_fp!=NULL)
			fclose(old_fp);
		snprintf(cmd,100,"rm %s",OLD_CONFIG_PATH(NTP_PATH));
		cmdRun(cmd);		
		startNTP(NULL);
		return "error:failed to read config file";
	}
	
	char line[MAXLENGTH];
	char isAdd = 0;//检测是否执行了添加语句，用于如果配置文件中本身没有server，将在配置文件末尾添加server
	while(fgets(line,MAXLENGTH,old_fp)){
		//修改公共配置
		if(strstr(line,"maxpoll")){
			if(info->maxpoll>0)
				snprintf(line,MAXLENGTH,"maxpoll %d\n",info->maxpoll);
			else if(strncmp(line,"#",1)!=0){
				char tmpline[200];
				strncpy(tmpline,line,200);
				snprintf(line,MAXLENGTH,"#%s",tmpline);
			}
		}
		
		if(strstr(line,"interface listen")){
			do{
				if(strlen(line) > 23){//源接口
					if(strlen(info->srcaddr)>0){//参数有的话修改,没有的话添加注释
						snprintf(line,MAXLENGTH,"interface listen %s\n",info->srcaddr);
					}else if(strncmp(line,"#",1)!=0){
						char tmpline[200];
						strncpy(tmpline,line,200);
						snprintf(line,MAXLENGTH,"#%s",tmpline);
					}
					strcpy(info->srcaddr,"");
				}else{//源地址
					if(strlen(info->interface)>0){
						snprintf(line,MAXLENGTH,"interface listen %s\n",info->interface);
					}else if(strncmp(line,"#",1)!=0){
						char tmpline[200];
						strncpy(tmpline,line,200);
						snprintf(line,MAXLENGTH,"#%s",tmpline);
					}
					strcpy(info->interface,"");
				}	
				fputs(line,new_fp);
				memset(line,0,MAXLENGTH);		
			}while(fgets(line,MAXLENGTH,old_fp) && strstr(line,"interface listen"));
				
			char tmpline[200];
			strncpy(tmpline,line,200);
			
			if(strlen(info->srcaddr)>0){
				snprintf(line,MAXLENGTH,"interface listen %s\n",info->srcaddr);
				fputs(line,new_fp);
				memset(line,0,MAXLENGTH);
			}
			if(strlen(info->interface)>0){
				snprintf(line,MAXLENGTH,"interface listen %s\n",info->interface);
				fputs(line,new_fp);
				memset(line,0,MAXLENGTH);
			}
			
			fputs(tmpline,new_fp);
			memset(line,0,MAXLENGTH);
			continue;
		}
		
		
		
		//这里会将连续读取server字段。并检测参数里面的server有没有和已存在的一致，如果有那就按参数修改已存的server
		if(isAdd==0 && strncmp(line,"server",strlen("server"))==0){
			do{
				char tmpserver[100];
				sscanf(line,"%*s %s",tmpserver);
				for(int i=0;i<info->serverNum;i++){
					if(strlen(((ntpserver*)info->servers)[i].server)>0 && strcmp(tmpserver,((ntpserver*)info->servers)[i].server)==0){
						if(((ntpserver*)info->servers)[i].ismain == 1){
							snprintf(line,MAXLENGTH,"server %s iburst",tmpserver);
						}else{
							snprintf(line,MAXLENGTH,"server %s",tmpserver);
						}
						
						strncpy(tmpserver,line,100);
						
						if(strlen(((ntpserver*)info->servers)[i].port)>0){
							snprintf(line,MAXLENGTH,"%s port %s",tmpserver,((ntpserver*)info->servers)[i].port);
						}else{
							snprintf(line,MAXLENGTH,"%s",tmpserver);
						}
						
						strcpy(((ntpserver*)info->servers)[i].server,"");//这里将修改的置空，避免将修改的添加为新的server
						break;
					}
				}
				strcat(line,"\n");	
				fputs(line,new_fp);
				memset(line,0,MAXLENGTH);				
			}while(fgets(line,MAXLENGTH,old_fp) && strncmp(line,"server",strlen("server"))==0);

			char tmpline[200];//将server字段的下一行暂存到tmpline，先添加新的server字段，最后在添加tmpline
			strncpy(tmpline,line,200);
			
			//server字段已被读完，下面开始添加新的server
			for(int i=0;i<info->serverNum;i++){
				if(strlen(((ntpserver*)info->servers)[i].server)<=0)
					continue;
					
				snprintf(line,MAXLENGTH,"server %s",((ntpserver*)info->servers)[i].server);
				
				if(((ntpserver*)info->servers)[i].ismain == 1){
					char tmp[200];
					strncpy(tmp,line,200);
					snprintf(line,MAXLENGTH,"%s iburst",tmp);
				}
				if(strlen(((ntpserver*)info->servers)[i].port)>0){
					char tmp[200];
					strncpy(tmp,line,200);
					snprintf(line,MAXLENGTH,"%s port %s",tmp,((ntpserver*)info->servers)[i].port);
				}
				strcat(line,"\n");	
				fputs(line,new_fp);
				memset(line,0,MAXLENGTH);
			}
			
			isAdd = 1;
			strncpy(line,tmpline,200);
		}
		
		fputs(line,new_fp);
		memset(line,0,MAXLENGTH);
	}
	
	//如果文件读取结束了isAdd还是为0，说明配置文件中没有server，则这里在文件末尾添加server
	if(isAdd == 0){
		for(int i=0;i<info->serverNum;i++){
			snprintf(line,MAXLENGTH,"server %s",((ntpserver*)info->servers)[i].server);
			
			if(((ntpserver*)info->servers)[i].ismain == 1){
				char tmp[200];
				strncpy(tmp,line,200);
				snprintf(line,MAXLENGTH,"%s iburst",tmp);
			}
			if(strlen(((ntpserver*)info->servers)[i].port)>0){
				char tmp[200];
				strncpy(tmp,line,200);
				snprintf(line,MAXLENGTH,"%s port %s",tmp,((ntpserver*)info->servers)[i].port);
			}
			
			strcat(line,"\n");
			fputs(line,new_fp);
			memset(line,0,MAXLENGTH);
		}		
	}
	
	fclose(new_fp);
	fclose(old_fp);
	startNTP(NULL);
	snprintf(cmd,100,"rm %s",OLD_CONFIG_PATH(NTP_PATH));
	cmdRun(cmd);
	return NULL;
}

void* delNTPServer(void *arg){
	paraData *data = NULL;
	jsonToParam(arg,&data);
	if(data == NULL){
		return "error:failed to read param";
	}
	ntpinfo *infoArray = data->dataArray;
	ntpinfo *info = &infoArray[0];
	FILE *new_fp=NULL,*old_fp=NULL;
	char cmd[100];
	snprintf(cmd,100,"cp %s %s",NTP_PATH,OLD_CONFIG_PATH(NTP_PATH));
	cmdRun(cmd);
	stopNTP(NULL);
	new_fp = fopen(NTP_PATH,"w");
	old_fp = fopen(OLD_CONFIG_PATH(NTP_PATH),"r");
	if(new_fp==NULL || old_fp==NULL){
		if(new_fp!=NULL)
			fclose(new_fp);
		if(old_fp!=NULL)
			fclose(old_fp);
		snprintf(cmd,100,"rm %s",OLD_CONFIG_PATH(NTP_PATH));
		cmdRun(cmd);		
		startNTP(NULL);
		return "error:failed to read config file";
	}
	char line[MAXLENGTH];
	while(fgets(line,MAXLENGTH,old_fp)){
		if(strncmp(line,"server",strlen("server"))==0){
			char tmpserver[30];
			sscanf(line,"%*s %s",tmpserver);
			for(int i=0;i<info->serverNum;i++){
				if(strcmp(tmpserver,((ntpserver*)info->servers)[i].server)==0){
					memset(line,0,MAXLENGTH);
					continue;
				}
			}
			
		}		
		fputs(line,new_fp);
		memset(line,0,MAXLENGTH);
	}
	
	fclose(new_fp);
	fclose(old_fp);
	startNTP(NULL);
	snprintf(cmd,100,"rm %s",OLD_CONFIG_PATH(NTP_PATH));
	cmdRun(cmd);
	return NULL;
}

void* startNTP(void *arg){
	if(is_process_running("ntpd")){
		printf("ntpd is running\n");
		return NULL;
	}
	system("/etc/init.d/ntpd start");
	return NULL;
}

void* stopNTP(void *arg){
	system("killall ntpd");
	printf("ntpd has been shut down\n");
	return NULL;
}