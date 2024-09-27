#include "common.h"


/**
-1:错误
0 :没有运行
1 :运行
*/

int is_process_running(char *processName){
	FILE *fp;
	char command[MAXLENGTH];
	char line[MAXLENGTH];
	//arm板子上直接使用ps即可，但是pc上要使用ps -a
	sprintf(command,"ps | grep %s | grep -v grep",processName);
	fp = popen(command,"r");
	if(fp==NULL){
		perror("In is_process_running , popen error");
		return -1;	
	}
	while(fgets(line,MAXLENGTH,fp) != NULL){
		//printf("%s",line);
		if(strstr(line,processName)!=NULL){
			pclose(fp);
			return 1;//程序正在运行		
		}
	}
	pclose(fp);
	return 0;//程序没在运行
}


int cmdRun(char *command){
	FILE *fp;
	char line[MAXLENGTH];
	fp = popen(command,"r");
	if(!fp){
		perror("In function cmcdRUN,failed to popen");
		return -1;	
	}
	while(fgets(line,MAXLENGTH,fp) != NULL){
		printf("%s",line);
	}
	pclose(fp);
	return 0;
}

int delFile(char *path){
	char cmd[200];
	snprintf(cmd,200,"rm %s",path);
	return cmdRun(cmd);
}




/*
int main(int argc,char **argv){
	printf("%s:%d\n",argv[1],is_process_running(argv[1]));

	return 0;
}*/
