#include "ssh.h"


void* startssh(void* arg){
    FILE* fp=NULL;
    char cmd[200];
    char line[1024];
    snprintf(cmd,200,"/etc/init.d/sshd start",NULL);
    fp = popen(cmd,"r");
    if(fp == NULL){
        return "error:failed to create popen";
    }
    while(fgets(line,1024,fp)!=NULL){
        printf("%s\n",line);
        memset(line,0,1024);
    }
    pclose(fp);
    return NULL;
}
void* stopssh(void* arg){
    FILE* fp=NULL;
    char cmd[200];
    char line[1024];
    snprintf(cmd,200,"/etc/init.d/sshd stop",NULL);
    fp = popen(cmd,"r");
    if(fp == NULL){
        return "error:failed to create popen";
    }
    while(fgets(line,1024,fp)!=NULL){
        printf("%s\n",line);
        memset(line,0,1024);
    }
    pclose(fp);
    return NULL;
}
void* setssh(void* arg){
    paraData *data = NULL;
    int arraySize = jsonToParam(arg,&data);
    sshinfo *sshinfoArray = data->dataArray;
    sshinfo *info = &sshinfoArray[0];

    stopssh(NULL);
    FILE* new_fp=NULL;
    FILE* old_fp=NULL;
    char cmd[200];
    snprintf(cmd,200,"mv %s %s",SSH_CONFIG_PATH,OLD_CONFIG_PATH(SSH_CONFIG_PATH));
    system(cmd);
    snprintf(cmd,200,"touch %s",SSH_CONFIG_PATH);
    system(cmd);

    new_fp = fopen(SSH_CONFIG_PATH,"w");
    old_fp = fopen(SSH_CONFIG_PATH,"r");
    if(old_fp==NULL || new_fp==NULL){
        startssh(NULL);
        return "error:failed to open ssh.conf";
    }
    char line[1024];
	char falg = 0;//用来表示是否都已成功改写line
    while(fgets(line,1024,old_fp)){
		if(falg<3){
			if(strstr(line,"ListenAddress") && !strstr(line,"ListenAddress ::") && info->listenaddr){//改写line
				if(strcmp(info->listenaddr,"Any")==0){
					char tmpline[200];
					strncpy(tmpline,line,sizeof(line));
					snprintf(line,1024,"# %s",tmpline);
				}else{
					snprintf(line,1024,"ListenAddress %s",info->listenaddr);
				}
				falg++;
			}
			if(strstr(line,"Port") && !strstr(line,"GatewayPorts") && info->port){
				snprintf(line,1024,"Port %s",info->port);
				falg++;
			}
			if(strstr(line,"ClientAliveInterval") && info->outtime){
				snprintf(line,1024,"ClientAliveInterval %s",info->outtime);
				falg++;
			}
		}
        fputs(line,new_fp);
        memset(line,0,1024);
    }
    fclose(old_fp);
    fclose(new_fp);

    snprintf(cmd,200,"rm %s.old",OLD_CONFIG_PATH(SSH_CONFIG_PATH));
    system(cmd);
    startssh(NULL);
    return NULL;
}

