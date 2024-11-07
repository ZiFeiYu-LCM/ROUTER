#include "firewall.h"
//这里注意，iptables添加修改防火墙规则，默认是保存到内存，开机后没有了，所以要使用iptables-save > /etc/firewall保存，
//开机时候使用iptables-restore < /etc/firewall恢复配置

void *addFirewallRule(void* arg){
	printf("debug 4.1\n");
	char *res=NULL;
	paraData *data = NULL;
	int tmpNum = jsonToParam(arg,&data);
	printf("debug 4.2\n");
	if(tmpNum<=0){
		return "error:failed to read param";
	}
	firewallrule *infoArray = data->dataArray;
	firewallrule *info = &infoArray[0];
	
	
	printf("firewall : id:%d   port:%s",info->id,info->table);
	
	
	
	char cmd[200];
	snprintf(cmd,100,"iptables -A %s",info->table);

	if(info->srcip){
		strcat(cmd," -s ");
		strcat(cmd,info->srcip);
	}
	if(info->desip){
		strcat(cmd," -d ");
		strcat(cmd,info->desip);		
	}
	if(info->protocol){
		strcat(cmd," -p ");
		strcat(cmd,info->protocol);		
	}
	if(info->port){
		strcat(cmd," --dport ");
		strcat(cmd,info->port);		
	}
	if( strcmp(info->table,"OUTPUT")!=0 && info->in_interface){
		strcat(cmd," -i ");
		strcat(cmd,info->in_interface);			
	}
	if( strcmp(info->table,"INPUT")!=0 &&info->out_interface){
		strcat(cmd," -o ");
		strcat(cmd,info->out_interface);			
	}
	
	strcat(cmd," -j ");
	strcat(cmd,info->jump);
	printf("cmd = %s\n",cmd);
	cmdRun(cmd);
	paramFree(data);
	cmdRun("iptables-save");
	return res;
}

void *delFirewallRule(void* arg){
	char *res=NULL;
	paraData *data = NULL;
	int tmpNum = jsonToParam(arg,&data);
	if(tmpNum<=0){
		return "error:failed to read param";
	}
	firewallrule *infoArray = data->dataArray;
	firewallrule *info = &infoArray[0];
	
	printf("firewall : id:%d   port:%s",info->id,info->table);
	
	char cmd[200];
	snprintf(cmd,100,"iptables -D %s",info->table);

	if(info->srcip){
		strcat(cmd," -s ");
		strcat(cmd,info->srcip);
	}
	if(info->desip){
		strcat(cmd," -d ");
		strcat(cmd,info->desip);		
	}
	if(info->protocol){
		strcat(cmd," -p ");
		strcat(cmd,info->protocol);		
	}
	if(info->port){
		strcat(cmd," --dport ");
		strcat(cmd,info->port);		
	}
	if( strcmp(info->table,"OUTPUT")!=0 && info->in_interface){
		strcat(cmd," -i ");
		strcat(cmd,info->in_interface);			
	}
	if( strcmp(info->table,"INPUT")!=0 &&info->out_interface){
		strcat(cmd," -o ");
		strcat(cmd,info->out_interface);			
	}
	
	strcat(cmd," -j ");
	strcat(cmd,info->jump);
	printf("cmd = %s\n",cmd);
	cmdRun(cmd);
	paramFree(data);
	cmdRun("iptables-save");
	return res;
}

char *getFirewallRule(void* arg){
	char *topic="net/getFireRule";
	char *res = NULL;
	FILE *fp=NULL;
	char cmd[100];
	snprintf(cmd,200,"iptables -S",NULL);
//	snprintf(cmd,200,"iptables -S | grep \"^-A INPUT\"",NULL);
	fp = popen(cmd,"r");
	if(fp==NULL){
		return "error:failed to create popen";
	}
	char line[1024];
	int number = 0;
	while(fgets(line,1024,fp)!=0){
		if(!strstr(line, "-j")){
			memset(line,0,1024);
			continue;
		}
		number++;
		memset(line,0,1024);
	}
	pclose(fp);
	
	firewallrule *infoArray = (firewallrule*)malloc(sizeof(firewallrule)*number);
	paraData *data = paramInit(topic,number,200);
	number = 0;
	fp = popen(cmd,"r");
	if(fp==NULL){
		return "error:failed to create popen";
	}
	while(fgets(line,1024,fp)!=0){
		const char *ptr;
		if(!strstr(line, "-j")){
			memset(line,0,1024);
			continue;
		}
		printf("line : %s\n",line);
		if ((ptr = strstr(line, "-A ")) != NULL) {
			sscanf(ptr, "-A %s", &infoArray[number].table);
		}
		if ((ptr = strstr(line, "-s ")) != NULL) {
			sscanf(ptr, "-s %s", &infoArray[number].srcip);
		}
		if ((ptr = strstr(line, "-d ")) != NULL) {
			sscanf(ptr, "-d %s", &infoArray[number].desip);
		}
		if ((ptr = strstr(line, "-i ")) != NULL) {
			sscanf(ptr, "-i %s", &infoArray[number].in_interface);
		}
		if ((ptr = strstr(line, "-o ")) != NULL) {
			sscanf(ptr, "-o %s", &infoArray[number].out_interface);
		}
		if ((ptr = strstr(line, "-p ")) != NULL) {
			sscanf(ptr, "-p %s", &infoArray[number].protocol);
		}
//		if ((ptr = strstr(line, "-m ")) != NULL) {
//			sscanf(ptr, "-m %s", parsed_rule->match);
//		}
		if ((ptr = strstr(line, "--dport ")) != NULL) {
			sscanf(ptr, "--dport %s", &infoArray[number].port);
		}
		if ((ptr = strstr(line, "-j ")) != NULL) {
			sscanf(ptr, "-j %s", &infoArray[number].jump);
		}		

		
		memset(line,0,1024);
		number++;
	}
	pclose(fp);
	printf("table = %s\n",infoArray[0].table);
	printf("srcip = %s\n",infoArray[0].srcip);
	printf("port = %s\n",infoArray[0].port);
	data->dataArray = infoArray;
	res = paramToJson(data);
	paramFree(data);
	return res;
}