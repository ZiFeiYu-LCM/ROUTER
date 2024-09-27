#include "plcctl.h"
#include <snap7.h>
#include "../mqtt.h"
char pollDevlist_stopFlag = 0;
char plcFlag = 0; 
char timerStopFlag = 1;//定时器关闭 
int connectPLC(void *p,S7Object *client,char wrFlag);
void disconnect(S7Object *client);
int _readValue(S7Object *client,int dbnum, int offset,void *data,unsigned char flag);
void readValue8(S7Object *client,int offset,void* data);
void readValue16(S7Object *client,int offset,void* data);
void readValue32(S7Object *client,int offset,void* data);

int _writeValue(S7Object *client,int dbnum,int offset,void *data,unsigned char flag);
int writeValue8(S7Object *client,int offset,void *data);
int writeValue16(S7Object *client,int offset,void *data);
int writeValue32(S7Object *client,int offset,void *data);
int writeValue32f(S7Object *client,int offset,void *data);

/************************************************************************************************
				                     plcctl调用的函数
************************************************************************************************/
void *getMpinfoList_update(char *jsonStr){
	paraData* data = NULL;
	int tmpNum = jsonToParam(jsonStr,&data);
	if(tmpNum <= 0){
		return "error:jsonStr is NULL\n";
	}
	
	pollinfo* updateArray = (pollinfo*)(data->dataArray);
	
	for(int i=0;i<tmpNum;i++){
		
		struct pollNode* orgNode = selectNodefromList(pollList,&updateArray[i]);
		pollinfo* orgNodeinfo = (pollinfo*)malloc(sizeof(pollinfo));
		memcpy(orgNodeinfo,orgNode->node,sizeof(pollinfo));
		
		struct pollNode *tmpNode = pollListUpdate(pollList,&updateArray[i]);		
		//////////		
		printf("=====更新节点前的遍历   getMpinfoList_update\n");
		struct pollNode* tmpNode1 = tmpNode;
		printf("%s_%s:	timer1:%ld  timer2:%ld  uploadTimer:%ld\n",tmpNode1->node->devname,tmpNode1->node->mpname,
		tmpNode1->node->timer.readTimer1,tmpNode1->node->timer.readTimer2,tmpNode1->node->timer.uploadTimer);
		//////////
		pthread_mutex_lock(&(tmpNode->mutex));


		//重新设置定时器时间
		if(tmpNode->node->pollcycle != orgNodeinfo->pollcycle){
			tmpNode->node->timer.its1.it_value.tv_sec = tmpNode->node->pollcycle;
			tmpNode->node->timer.its1.it_interval.tv_sec = tmpNode->node->pollcycle;
			if(!timerStopFlag){
				if ((tmpNode->node->timer.its1.it_value.tv_sec==0 && tmpNode->node->timer.its1.it_interval.tv_sec==0) || (timer_settime(tmpNode->node->timer.readTimer1, 0, &tmpNode->node->timer.its1, NULL)==-1)) {//重新设置定时器的出发时间
					char tmpstr[200];
					snprintf(tmpstr,200,"%s_%s failed to start readTimer1",tmpNode->node->devname,tmpNode->node->mpname);
					LOG_ERROR(tmpstr);
				}
			}
		}
		
		if(tmpNode->node->pollcycle2 != orgNodeinfo->pollcycle2){
			tmpNode->node->timer.its2.it_value.tv_sec = tmpNode->node->pollcycle2;
			tmpNode->node->timer.its2.it_interval.tv_sec = tmpNode->node->pollcycle2;
			if(!timerStopFlag && tmpNode->node->ismultipoll){
				if ((tmpNode->node->timer.its2.it_value.tv_sec==0 && tmpNode->node->timer.its2.it_interval.tv_sec==0) || (timer_settime(tmpNode->node->timer.readTimer2, 0, &tmpNode->node->timer.its2, NULL)==-1)) {//重新设置定时器的出发时间
					char tmpstr[200];
					snprintf(tmpstr,200,"%s_%s failed to start readTimer2",tmpNode->node->devname,tmpNode->node->mpname);
					LOG_ERROR(tmpstr);
				}
			}
		}
		
		if(tmpNode->node->uploadperiod != orgNodeinfo->uploadperiod){
			tmpNode->node->timer.uploadIts.it_value.tv_sec = tmpNode->node->uploadperiod;
			tmpNode->node->timer.uploadIts.it_interval.tv_sec = tmpNode->node->uploadperiod;
			if(!timerStopFlag){
				if ((tmpNode->node->timer.uploadIts.it_value.tv_sec==0 && tmpNode->node->timer.uploadIts.it_interval.tv_sec==0) || (timer_settime(tmpNode->node->timer.uploadTimer, 0, &tmpNode->node->timer.uploadIts, NULL)==-1)) {//重新设置定时器的出发时间
					char tmpstr[200];
					snprintf(tmpstr,200,"%s_%s failed to start readTimer1",tmpNode->node->devname,tmpNode->node->mpname);
					LOG_ERROR(tmpstr);
				}	
			}
		}
		
		//////////		
		printf("=====更新节点中的遍历   getMpinfoList_update\n");
		tmpNode1 = tmpNode;
		printf("%s_%s:	timer1:%ld  timer2:%ld  uploadTimer:%ld\n",tmpNode1->node->devname,tmpNode1->node->mpname,
		tmpNode1->node->timer.readTimer1,tmpNode1->node->timer.readTimer2,tmpNode1->node->timer.uploadTimer);
		//////////

		pthread_mutex_unlock(&(tmpNode->mutex));
		if(orgNodeinfo!=NULL)
			free(orgNodeinfo);
	}
	if(updateArray!=NULL)
		free(updateArray);
	return NULL;
}
void *getMpinfoList_delete(char *jsonStr){//删除本地的某个节点信息
	paraData* data = NULL;
	int tmpNum = jsonToParam(jsonStr,&data);
	if(tmpNum <= 0){
		return "error:jsonStr is NULL\n";
	}
	pollinfo* delArray = (pollinfo*)(data->dataArray);
	
	for(int i=0;i<tmpNum;i++){
		struct pollNode* tmpNode = selectNodefromList(pollList, &delArray[i]);
		pthread_mutex_lock(&(tmpNode->mutex));
		
		//销毁定时器
		if (timer_delete(tmpNode->node->timer.readTimer1) == -1) {
			char tmpstr[200];
			snprintf(tmpstr,200,"%s_%s failed to stop readTimer1",tmpNode->node->devname,tmpNode->node->mpname);
			LOG_ERROR(tmpstr);
		}else{
			printf("已删除%s定时器readTimer1:  timerid:%ld\n",tmpNode->node->mpname,tmpNode->node->timer.readTimer1);
		}

		if (timer_delete(tmpNode->node->timer.readTimer2) == -1) {
			char tmpstr[200];
			snprintf(tmpstr,200,"%s_%s failed to stop readTimer2",tmpNode->node->devname,tmpNode->node->mpname);
			LOG_ERROR(tmpstr);
		}else{
			printf("已删除%s定时器readTimer2   timerid:%ld\n",tmpNode->node->mpname,tmpNode->node->timer.readTimer2);
		}
		
		if (timer_delete(tmpNode->node->timer.uploadTimer) == -1) {
			char tmpstr[200];
			snprintf(tmpstr,200,"%s_%s failed to stop uploadTimer",tmpNode->node->devname,tmpNode->node->mpname);
			LOG_ERROR(tmpstr);
		}else{
			printf("已删除%s定时器uploadTimer   timerid:%ld\n",tmpNode->node->mpname,tmpNode->node->timer.uploadTimer);
		}
		
		
		//删除节点时，删除对应的文件
		char *logFile = malloc(sizeof(LOGPATH)+sizeof(tmpNode->node->devname)+sizeof(tmpNode->node->mpname)+7);
		sprintf(logFile,"%s%s_%s.log",LOGPATH,tmpNode->node->devname,tmpNode->node->mpname);
		//	char *logFile = malloc(sizeof(LOGPATH)+sizeof(info->devid)+sizeof(info->mpid)+7);//以id为路径
		//	sprintf(logFile,"%s%d_%d.log",LOGPATH,info->devid,info->mpid);
		if (access(logFile, F_OK) == 0) {//若文件存在
			FILE *fp;
			char cmd[1024];
			snprintf(cmd,1024,"rm %s",logFile);
			fp = popen(cmd,"r");
		    if (fp == NULL) {
				LOG_ERROR("popen failed");
			}else{
				char cmdRes[1024];
				while(fgets(cmdRes,1024,fp)!=NULL){
					printf(cmdRes);
				}
				if (pclose(fp) == -1) {
					LOG_ERROR("pclose failed");
				}
			}
		}
		free(logFile);
		
		pthread_mutex_unlock(&(tmpNode->mutex));
		pollListDel(pollList, &delArray[i]);
	}
	
	if(delArray!=NULL)
		free(delArray);
	
	printf("删除已运行完毕\n");
	listIterator(pollList);
	
	return NULL;
}
void *getMpinfoList_add(char *jsonStr){
	paraData* data = NULL;
	int tmpNum = jsonToParam(jsonStr,&data);
	if(tmpNum <= 0){
		return "error:jsonStr is NULL\n";
	}
	pollinfo* addArray = (pollinfo*)(data->dataArray);
	
	for(int i=0;i<tmpNum;i++){
		pollinfo* nodeinfo = (pollinfo*)malloc(sizeof(pollinfo));
		memcpy(nodeinfo,&addArray[i],sizeof(pollinfo));
		struct pollNode* tmpNode = pollListAdd(pollList,nodeinfo);
		timerOpsSet(tmpNode);
		
		if(!timerStopFlag){//是否开启定时器
			//printf("正在启动定时器...\n");
			if ((nodeinfo->timer.its1.it_value.tv_sec==0 && nodeinfo->timer.its1.it_interval.tv_sec==0) || (timer_settime(nodeinfo->timer.readTimer1, 0, &nodeinfo->timer.its1, NULL) == -1)) {
				char tmpstr[200];
				snprintf(tmpstr,200,"%s_%s failed to start readTimer1",nodeinfo->devname,nodeinfo->mpname);
				LOG_ERROR(tmpstr);
			}else{
				printf("%s_%s:	timer1:%ld\n",tmpNode->node->devname,tmpNode->node->mpname,
		tmpNode->node->timer.readTimer1);
			}
		
			if(nodeinfo->ismultipoll){
				if ((nodeinfo->timer.its2.it_value.tv_sec==0 && nodeinfo->timer.its2.it_interval.tv_sec==0) || (timer_settime(nodeinfo->timer.readTimer2, 0, &nodeinfo->timer.its2, NULL) == -1)) {
					char tmpstr[200];
					snprintf(tmpstr,200,"%s_%s failed to start readTimer2",nodeinfo->devname,nodeinfo->mpname);
					LOG_ERROR(tmpstr);
				}else{
				printf("%s_%s:	timer2:%ld\n",tmpNode->node->devname,tmpNode->node->mpname,
		tmpNode->node->timer.readTimer2);
			}		
			}
		
			if ((nodeinfo->timer.uploadIts.it_value.tv_sec==0 && nodeinfo->timer.uploadIts.it_interval.tv_sec==0) || (timer_settime(nodeinfo->timer.uploadTimer, 0, &nodeinfo->timer.uploadIts, NULL) == -1)) {
					char tmpstr[200];
					snprintf(tmpstr,200,"%s_%s failed to start uploadTimer",nodeinfo->devname,nodeinfo->mpname);
					LOG_ERROR(tmpstr);
			}else{
				printf("%s_%s:	uploadTimer:%ld\n",tmpNode->node->devname,tmpNode->node->mpname,
		tmpNode->node->timer.uploadTimer);
			}
		}
	}
	
	listIterator(pollList);
	
	return NULL;
}
void *getMpinfoList(char *jsonStr){
	paraData* data = NULL;
	int tmpNum = jsonToParam(jsonStr,&data);
	if(tmpNum < 0){
		return "error:jsonStr is NULL\n";
	}
	
	if(pollList!=NULL)
		freeList(pollList);
		
	pollInfoArraySize = tmpNum;
	pollInfoArray = (pollinfo*)(data->dataArray);
	
	pollList = pollListInit();
	for(int i=0;i<pollInfoArraySize;i++){
		pollinfo* nodeinfo = (pollinfo*)malloc(sizeof(pollinfo));
		
		memcpy(nodeinfo, &pollInfoArray[i], sizeof(pollinfo));
		struct pollNode* tmpNode = pollListAdd(pollList,nodeinfo);
		timerOpsSet(tmpNode);
	}

	freeMpinfoArray(NULL);
	if(data!=NULL){
		if(data->msg!=NULL){
			free(data->msg);
			data->msg = NULL;
		}

		if(pollInfoArray == NULL){
			data->dataArray = NULL;
		}else{
			free(data->dataArray);
			data->dataArray = NULL;
		}

		free(data);
		data = NULL;
	}
	
	
//	listIterator(pollList);
	
//	struct pollNode* tmpNode = pollList->head;
//	while(tmpNode!=NULL){
//		printf("%s_%s:	timer1:%ld  timer2:%ld  uploadTimer:%ld\n",tmpNode->node->devname,tmpNode->node->mpname,
//		tmpNode->node->timer.readTimer1,tmpNode->node->timer.readTimer2,tmpNode->node->timer.uploadTimer);
//		
//		tmpNode = tmpNode->next;
//	}
	
	printf("getMpinfoList over!\n");
	return NULL;
}
void *freeMpinfoArray(char *arg){
//	timerStopFlag = 1;

	if(pollInfoArraySize>0){
		free(pollInfoArray);
		pollInfoArray = NULL;
		pollInfoArraySize = 0;
	}
	printf("pollInfoArray successful\n");
	return NULL;
}
int timerOpsSet(struct pollNode* node){
	pollinfo* nodeinfo = node->node;
	//配置定时器1
	nodeinfo->timer.sev1.sigev_notify = SIGEV_THREAD;
	nodeinfo->timer.sev1.sigev_value.sival_ptr = node;
	nodeinfo->timer.sev1.sigev_notify_function = timer_handler;
	nodeinfo->timer.sev1.sigev_notify_attributes = NULL;

	nodeinfo->timer.its1.it_value.tv_sec = nodeinfo->pollcycle;
	nodeinfo->timer.its1.it_value.tv_nsec = 0;
	nodeinfo->timer.its1.it_interval.tv_sec = nodeinfo->pollcycle;
	nodeinfo->timer.its1.it_interval.tv_nsec = 0;	
	if (timer_create(CLOCK_REALTIME, &(nodeinfo->timer.sev1), &(nodeinfo->timer.readTimer1)) == -1) {
			char tmpstr[200];
			snprintf(tmpstr,200,"%s_%s failed to create readTimer1",nodeinfo->devname,nodeinfo->mpname);
			LOG_ERROR(tmpstr);
	}


	//配置定时器2
	nodeinfo->timer.sev2.sigev_notify = SIGEV_THREAD;
	nodeinfo->timer.sev2.sigev_value.sival_ptr = node;
	nodeinfo->timer.sev2.sigev_notify_function = timer_handler;
	nodeinfo->timer.sev2.sigev_notify_attributes = NULL;

	nodeinfo->timer.its2.it_value.tv_sec = nodeinfo->pollcycle2;
	nodeinfo->timer.its2.it_value.tv_nsec = 0;
	nodeinfo->timer.its2.it_interval.tv_sec = nodeinfo->pollcycle2;
	nodeinfo->timer.its2.it_interval.tv_nsec = 0;	
	if (timer_create(CLOCK_REALTIME, &(nodeinfo->timer.sev2), &(nodeinfo->timer.readTimer2)) == -1) {
		char tmpstr[200];
		snprintf(tmpstr,200,"%s_%s failed to create readTimer2",nodeinfo->devname,nodeinfo->mpname);
		LOG_ERROR(tmpstr);
	}

	
	//配置上传定时器
	nodeinfo->timer.uploadSev.sigev_notify = SIGEV_THREAD;
	nodeinfo->timer.uploadSev.sigev_value.sival_ptr = node;
	nodeinfo->timer.uploadSev.sigev_notify_function = timer_uploadLog;
	nodeinfo->timer.uploadSev.sigev_notify_attributes = NULL;	
	
	nodeinfo->timer.uploadIts.it_value.tv_sec = nodeinfo->uploadperiod;
	nodeinfo->timer.uploadIts.it_value.tv_nsec = 0;
	nodeinfo->timer.uploadIts.it_interval.tv_sec = nodeinfo->uploadperiod;
	nodeinfo->timer.uploadIts.it_interval.tv_nsec = 0;
	if (timer_create(CLOCK_REALTIME, &(nodeinfo->timer.uploadSev), &(nodeinfo->timer.uploadTimer)) == -1) {
		char tmpstr[200];
		snprintf(tmpstr,200,"%s_%s failed to create uploadtimer",nodeinfo->devname,nodeinfo->mpname);
		LOG_ERROR(tmpstr);
	}
	return 0;
}


//从存值的日志文件中读取最后一行返回给web前端
char *putMpinfoList(char *jsonStr){//后端会先赛选出符合条件的测点数据，这里按数据直接查即可
	char *topic = "plc/putmpinfo";
	paraData* data = NULL;
	int listSize = jsonToParam(jsonStr,&data);
	if(listSize<=0){
		return NULL;
	}
//	pollinfo *infolist = data->dataArray; 
	readMpinfo *infolist = data->dataArray; 
	
	readMpinfo *mpinfoList = (readMpinfo*)malloc(sizeof(readMpinfo)*listSize);
	int resSize = 0;
	for(int i=0;i<listSize;i++){
		readMpinfo *info = &infolist[i];
		printf("info[%d]   %s  \n",i,info->devname);
		char *logFile = malloc(sizeof(LOGPATH)+sizeof(info->devname)+sizeof(info->mpname)+7);
		sprintf(logFile,"%s%s_%s.log",LOGPATH,info->devname,info->mpname);
//		char *logFile = malloc(sizeof(LOGPATH)+sizeof(info->devid)+sizeof(info->mpid)+7);//--------------用id当做路径
//		sprintf(logFile,"%s%d_%d.log",LOGPATH,info->devid,info->mpid);
		if (access(logFile, F_OK) == -1) {
			continue;
		}
		char lastLine[1024];
		getLastLine(logFile,lastLine);//将文件中最后一行存入lastline；
		char *valueStr = strtok(lastLine,"#");//获取lastline的value部分
		char *timeStr = strtok(NULL,"#");//获取lastline的time部分
		free(logFile);
		
		mpinfoList[resSize].devid = info->devid;
		mpinfoList[resSize].mpid = info->mpid;
		strcpy(mpinfoList[resSize].devname,info->devname);
		strcpy(mpinfoList[resSize].mpname,info->mpname);
		strcpy(mpinfoList[resSize].groupname,info->groupname);
		mpinfoList[resSize].valuetype = info->valuetype;
		strcpy(mpinfoList[resSize].address,info->address);
		strcpy(mpinfoList[resSize].mpnote,info->mpnote);
		strcpy(mpinfoList[resSize].ip,info->ip);
		strcpy(mpinfoList[resSize].value,valueStr);
		strcpy(mpinfoList[resSize].time,timeStr);
		resSize++;
	}
	
	paramFree(data);
	data = paramInit(topic,resSize,0);
	data->dataArray = mpinfoList;
	char *resJson = paramToJson(data);//返回数组json串
	//free(mpinfoList);//会在paramFree里面释放   data->dataArray = mpinfoList;
//	mpinfoList=NULL;

	paramFree(data);
	return resJson;
}

//存储
void saveData(void *data,pollinfo *info){//存储
	char *logFile = malloc(sizeof(LOGPATH)+sizeof(info->devname)+sizeof(info->mpname)+7);
	sprintf(logFile,"%s%s_%s.log",LOGPATH,info->devname,info->mpname,info->warnname);
	
//	char *logFile = malloc(sizeof(LOGPATH)+sizeof(info->devid)+sizeof(info->mpid)+7);//以id为路径
//	sprintf(logFile,"%s%d_%d.log",LOGPATH,info->devid,info->mpid);
	printf("logFile : %s\n",logFile);
	if (access(logFile, F_OK) == -1) {
        // log文件不存在
		FILE *file = fopen(logFile, "w");
		if (file == NULL) {
			perror("Failed to create file");
			return;
		}
		// 关闭文件
		fclose(file);
    }
	time_t now = time(NULL);
    // 格式化时间为字符串
    char timebuf[80];
    strftime(timebuf, sizeof(timebuf), "%Y-%m-%d %H:%M:%S", localtime(&now));
	char command[100];
	switch(info->valuetype){
		case 1:{
				uint8_t *value = (uint8_t*)data;
				snprintf(command, sizeof(command), "echo \"%d #%s\" >> %s", *value,timebuf,logFile);
			}
			break;
		case 2:{
				uint16_t *value = (uint16_t*)data;
				snprintf(command, sizeof(command), "echo \"%d #%s\" >> %s", *value,timebuf,logFile);
			}
			break;
		case 3:{
				uint32_t *value = (uint32_t*)data;
				snprintf(command, sizeof(command), "echo \"%d #%s\" >> %s", *value,timebuf,logFile);
			}
			break;
		case 4:{
				float *value = (float*)data;
				snprintf(command, sizeof(command), "echo \"%f #%s\" >> %s", *value,timebuf,logFile);
			}
			break;		
	}
	int res = system(command);
    if (res == -1) {
        perror("system echo failed to write");
        return;
    }
	//查看是否超出最大条目数，超出的话上传文件
	FILE *file = fopen(logFile,"r");
	int count = 0;
	char line[200];
	if(file == NULL){
		printf("failed to fopen %s to read!",logFile);
		return;
	}
	while(fgets(line,200,file)!=NULL){
		count++;
	}
	fclose(file);
	if(count >= info->maxitem){
		//上传文件
		printf("uploadlog  maxitems \n");
		uploadLog(logFile);
	}
	free(logFile);
}

//1==  2!=  3>  4>=  5<  6<=
char checkWarn(char conditionL,char conditionM,char conditionR,float value1,float value2,float value3){
	char res1,res2;
	//printf("conditionL:%d, conditionM:%d, conditionR:%d, value1=%f,value2=%f,value3=%f\n",conditionL,conditionM,conditionR,value1,value2,value3);
	switch(conditionL){
		case 1:{
			res1 = value1==value2?1:0;
		}break;
		case 2:{
			res1 = value1!=value2?1:0;
		}break;
		case 3:{
			res1 = value1>value2?1:0;
		}break;
		case 4:{
			res1 = value1>=value2?1:0;
		}break;
		case 5:{
			res1 = value1<value2?1:0;
		}break;
		case 6:{
			res1 = value1<=value2?1:0;
		}break;
		default:
		break;
	}
	if(conditionM){
		switch(conditionR){
			case 1:{
				res2 = value1==value3?1:0;
			}break;
			case 2:{
				res2 = value1!=value3?1:0;
			}break;
			case 3:{
				res2 = value1>value3?1:0;
			}break;
			case 4:{
				res2 = value1>=value3?1:0;
			}break;
			case 5:{
				res2 = value1<value3?1:0;
			}break;
			case 6:{
				res2 = value1<=value3?1:0;
			}break;
			default:
			break;
		}
		if(conditionM==1){
			return res1&&res2?1:0;
		}else if(conditionM==2){
			return res1||res2?1:0;
		}
	}else{
		return res1;
	}
	return 0;
}

//读取plc
void timer_handler(union sigval sv) {
	if(timerStopFlag==1)
		return;
	char warnConditionL,warnConditionM,warnConditionR;
	struct pollNode* tmpNode = (struct pollNode*)sv.sival_ptr;
	pollinfo *info = tmpNode->node;
	//printf("=============timer_handler: devid=%d mpid=%d devname=%s mpname=%s\n",info->devid,info->mpid,info->devname,info->mpname);
	
//	printf("				timer_handler\n");
//	listIterator(pollList);
//	printf("\n");
	
	pthread_mutex_lock(&(tmpNode->mutex));
//	printf("                     ++%s 上锁\n",info->mpname);
	warnConditionL = info->warncondition/100;
	warnConditionM = (info->warncondition%100)/10;
	warnConditionR = info->warncondition%10;
	

	char iswarn;
	switch(info->valuetype){
		case 1:{
				uint8_t data;
				if(readData(info,&data)){
					pthread_mutex_unlock(&(tmpNode->mutex));
//					printf("                     --%s 解锁\n",info->mpname);
					return;
				}	
				iswarn = checkWarn(warnConditionL,warnConditionM,warnConditionR,data,info->wcvalue1,info->wcvalue2);
				saveData((void*)&data,info);//存储
				printf("%s uint8_t=%d\n",info->address,data);
			}
			break;
		case 2:{
				uint16_t data;
				if(readData(info,&data)){
					pthread_mutex_unlock(&(tmpNode->mutex));
//					printf("                     --%s 解锁\n",info->mpname);
					return;
				}
				iswarn = checkWarn(warnConditionL,warnConditionM,warnConditionR,data,info->wcvalue1,info->wcvalue2);
				saveData((void*)&data,info);//存储
				printf("%s uint16_t=%d\n",info->address,data);
			}
			break;
		case 3:{
				uint32_t data;
				if(readData(info,&data)){
					pthread_mutex_unlock(&(tmpNode->mutex));
//					printf("                     --%s 解锁\n",info->mpname);
					return;
				}
				iswarn = checkWarn(warnConditionL,warnConditionM,warnConditionR,data,info->wcvalue1,info->wcvalue2);
				saveData((void*)&data,info);//存储
				printf("%s uint32_t=%d\n",info->address,data);
			}
			break;
		case 4:{
				float data;
				if(readData(info,&data)){
					pthread_mutex_unlock(&(tmpNode->mutex));
//					printf("                     --%s 解锁\n",info->mpname);
					return;
				}
				iswarn = checkWarn(warnConditionL,warnConditionM,warnConditionR,data,info->wcvalue1,info->wcvalue2);
				saveData((void*)&data,info);//存储
				printf("%s float = %f\n",info->address,data);
			}
			break;		
	}
	if(iswarn){
		printf("%s_%s_%s WARN!\n",info->devname,info->mpname,info->warnname);
		/*===========================================================================================================报警操作*/
	}
	pthread_mutex_unlock(&(tmpNode->mutex));
//	printf("                     --%s 解锁\n",info->mpname);
}

//线程函数，用来设置读取定时器    和   上传定时器
int _pollmp(void* arg) {
	if(pollList->listSize<=0){
		return 0;
	}

	printf("xxxxxxxxxxxx         _pollmp：\n");
	listIterator(pollList);
	printf("\n");
//
//	printf("开启定时器遍历：\n");
//	struct pollNode* tmpNode1 = pollList->head;
//	while(tmpNode1!=NULL){
//		printf("%s_%s:	timer1:%ld  timer2:%ld  uploadTimer:%ld\n",tmpNode1->node->devname,tmpNode1->node->mpname,
//		tmpNode1->node->timer.readTimer1,tmpNode1->node->timer.readTimer2,tmpNode1->node->timer.uploadTimer);
//		
//		tmpNode1 = tmpNode1->next;
//	}



	struct pollNode *tmpNode = pollList->head;
    //开启定时器
	while(tmpNode!=NULL){
		pthread_mutex_lock(&(tmpNode->mutex));
		pollinfo* nodeinfo = tmpNode->node;
		if ((nodeinfo->timer.its1.it_value.tv_sec==0 && nodeinfo->timer.its1.it_interval.tv_sec==0) || (timer_settime(nodeinfo->timer.readTimer1, 0, &nodeinfo->timer.its1, NULL) == -1)) {
			char tmpstr[200];
			snprintf(tmpstr,200,"%s_%s failed to start readTimer1",nodeinfo->devname,nodeinfo->mpname);
			LOG_ERROR(tmpstr);
		}
	
		if(nodeinfo->ismultipoll){
			if ((nodeinfo->timer.its2.it_value.tv_sec==0 && nodeinfo->timer.its2.it_interval.tv_sec==0) || (timer_settime(nodeinfo->timer.readTimer2, 0, &nodeinfo->timer.its2, NULL) == -1)) {
				char tmpstr[200];
				snprintf(tmpstr,200,"%s_%s failed to start readTimer2",nodeinfo->devname,nodeinfo->mpname);
				LOG_ERROR(tmpstr);
			}		
		}
	
		if ((nodeinfo->timer.uploadIts.it_value.tv_sec==0 && nodeinfo->timer.uploadIts.it_interval.tv_sec==0) || (timer_settime(nodeinfo->timer.uploadTimer, 0, &nodeinfo->timer.uploadIts, NULL) == -1)) {
				char tmpstr[200];
				snprintf(tmpstr,200,"%s_%s failed to start uploadTimer",nodeinfo->devname,nodeinfo->mpname);
				LOG_ERROR(tmpstr);
		}
		
		pthread_mutex_unlock(&(tmpNode->mutex));
		tmpNode = tmpNode->next;
	}
	
     //主循环
    while (!timerStopFlag);


	//停止定时器
	tmpNode = pollList->head;
	while(tmpNode!=NULL){
		pthread_mutex_lock(&(tmpNode->mutex));
		printf("		已上锁\n");
		pollinfo* nodeinfo = tmpNode->node;
		printf("定时器清除%s_%s\n",nodeinfo->devname,nodeinfo->mpname);
		if (timer_delete(nodeinfo->timer.readTimer1) == -1) {
			char tmpstr[200];
			snprintf(tmpstr,200,"%s_%s failed to stop readTimer1",nodeinfo->devname,nodeinfo->mpname);
			LOG_ERROR(tmpstr);
		}else{
			printf("		readTimer1 已经清除\n");
		}

		if (timer_delete(nodeinfo->timer.readTimer2) == -1) {
			char tmpstr[200];
			snprintf(tmpstr,200,"%s_%s failed to stop readTimer2",nodeinfo->devname,nodeinfo->mpname);
			LOG_ERROR(tmpstr);
		}else{
			printf("		readTimer2 已经清除\n");
		}
		
		if (timer_delete(nodeinfo->timer.uploadTimer) == -1) {
			char tmpstr[200];
			snprintf(tmpstr,200,"%s_%s failed to stop uploadTimer",nodeinfo->devname,nodeinfo->mpname);
			LOG_ERROR(tmpstr);
		}else{
			printf("		uploadTimer 已经清除\n");
		}
		
		pthread_mutex_unlock(&(tmpNode->mutex));
		printf("		已解锁\n");
		tmpNode = tmpNode->next;
	}
	printf("pollmp已运行完毕\n");
	
	return 0;
}

void *pollmp(void* arg){
	if(!timerStopFlag){
		printf("The plc service is running\n");
		return "error:the plc service is running";
	}
	if(pollList->listSize<=0){
		printf("pollList is NULL\n");
		return "error:pollList is NULL";
	}
	timerStopFlag = 0;
	pthread_t thread;
	int result = pthread_create(&thread, NULL, _pollmp, arg);
	if (result != 0) {
		printf("createPollpth: Error creating thread.\n");
		return -1;
	}
}

void *pollStop(void *arg){
//	printf("\nxxxxxxxxxxxx          pollStop：\n");
//	listIterator(pollList);
//	printf("\n");
	
	timerStopFlag = 1;
	printf("The plc service has been stopped\n");
	return NULL;
}


/**********************************************************************
				读写项
**********************************************************************/
void disconnect(S7Object *client){
	Cli_Disconnect(*client);
	Cli_Destroy(client);
}

//wrFlag表示读写，因为读传入的pollinfo，写传入的readMpinfo，所以用它来做判断， 0写  1读
int connectPLC(void *p,S7Object *client,char wrFlag){
	char istimeout=0,ip[50];
	int connecttime=0,collecttime=0;
	
	if(wrFlag){
		pollinfo* tmp = (pollinfo*)p;
		istimeout = tmp->istimeout;
		connecttime = tmp->connecttime;
		collecttime = tmp->collecttime;
		strncpy(ip,tmp->ip,50);
	}else{
		readMpinfo *tmp = (readMpinfo*)p;
		strncpy(ip,tmp->ip,50);
	}

//	printf("ip: %s\n",ip);
	int res = 0;
	*client = Cli_Create();
	if(*client == NULL){ // 检查是否成功分配内存
		printf("Failed to create client object\n");
		return -1;
	}
	
	/*******设置超时时间*********/	
	if(wrFlag && istimeout){
		// 设置连接超时时间为 5000 毫秒Timeout = 5000;
		res = Cli_SetParam(client, p_i32_PingTimeout, &connecttime);
		if (res != 0) {
			printf("Failed to set the connection timeout: %d\n", res);
		}
		// 设置接收超时时间为 5000 毫秒 
		res = Cli_SetParam(client, p_i32_RecvTimeout, &collecttime);
		if (res != 0) {
			printf("Failed to set the receiving timeout: %d\n", res);
		}
	}

	res = Cli_ConnectTo(*client, ip, rackNumber, slotNumber);
	if(res!=0){
		printf("Cli_ConnectTo error\n");	
		res = -1;	
	}else
		;
//		printf("Cli_ConnectTo success!\n");
	return res;
}

int _readValue(S7Object *client,int dbnum, int offset,void *data,unsigned char valueType){
	switch(valueType){
		case 8: 
			if(Cli_DBRead(*client,dbnum,offset,sizeof(*(uint8_t*)data),(uint8_t*)data)!=0){
				printf("failed DBRead8\n");		
				disconnect(client);
				return -1;
			}
			break;
		case 16: 
			if(Cli_DBRead(*client,dbnum,offset,sizeof(*(uint16_t*)data),(uint16_t*)data)!=0){
				printf("failed DBRead16\n");		
				disconnect(client);
				return -1;
			}
			break;
		case 32: 
			if(Cli_DBRead(*client,dbnum,offset,sizeof(*(uint32_t*)data),(uint32_t*)data)!=0){
				printf("failed DBRead32\n");		
				disconnect(client);
				return -1;
			}
			break;
	}
	return 0;
}

void readValue8(S7Object *client,int offset,void* data){
	_readValue(client,1, offset,data,8);
}
void readValue16(S7Object *client,int offset,void* data){
	_readValue(client,1, offset,data,16);
	*(uint16_t*)data = BIG_LITTLE_SWAP16(*data);
}
void readValue32(S7Object *client,int offset,void* data){
	_readValue(client,1, offset,data,32);
	*(uint32_t*)data = BIG_LITTLE_SWAP32(*data);
}
void readValue32f(S7Object *client,int offset,void* data){
	_readValue(client,1, offset,data,32);
	*(float*)data = uint32toFloat(BIG_LITTLE_SWAP32(*data));
}

int readData(pollinfo *info, void *data){
	S7Object myClient;//-------------------------------------------------------这里后面可能需要改进，每次都重新连接plc应该有点慢，应该同一连接，最后统一断开。
	if(connectPLC((void*)info,&myClient,1)<0){
		printf("connect to %s[%s] error!\n",info->devname,info->ip);
		return -1;	
	}
	switch(info->valuetype){
		case 1:{
			readValue8(&myClient,extractInteger(info->address),data);
		}break;
		case 2:{
			readValue16(&myClient,extractInteger(info->address),data);
		}break;
		case 3:{
			readValue32(&myClient,extractInteger(info->address),data);
		}break;
		case 4:{
			readValue32f(&myClient,extractInteger(info->address),data);
		}break;
		default:
		break;
	}
	disconnect(&myClient);
	return 0;
}

int _writeValue(S7Object *client,int dbnum,int offset,void *data,unsigned char flag){
	switch(flag){
		case 8:{
				if(Cli_DBWrite(*client,dbnum,offset,sizeof(*(uint8_t*)data),(uint8_t*)data)!=0){
					printf("failed DBWrite uint8_t\n");
					disconnect(client);
					return -1;			
				}
			}break;
		case 16:
			if(Cli_DBWrite(*client,dbnum,offset,sizeof(*(uint16_t*)data),(uint16_t*)data)!=0){
				printf("failed DBWrite uint16_t\n");
				disconnect(client);
				return -1;			
			}
			break;
		case 32:
			if(Cli_DBWrite(*client,dbnum,offset,sizeof(*(uint32_t*)data),(uint32_t*)data)!=0){
				printf("failed DBWrite uint32_t\n");
				disconnect(client);
				return -1;			
			}
			break;
		default:
			break;
	}
	return 0;
}

int writeValue8(S7Object *client,int offset,void *data){
	uint8_t tmp = *(uint8_t*)data;
	return _writeValue(client, 1, offset, &tmp, 8);
}
int writeValue16(S7Object *client,int offset,void *data){
	uint16_t tmp = *(uint16_t*)data;
	tmp = BIG_LITTLE_SWAP16(tmp);
	return _writeValue(client, 1, offset, &tmp, 16);
}
int writeValue32(S7Object *client,int offset,void *data){
	uint32_t tmp = *(uint32_t*)data;
	tmp = BIG_LITTLE_SWAP32(tmp);
	return _writeValue(client, 1, offset, &tmp, 32);
}
int writeValue32f(S7Object *client,int offset,void *data){
	uint32_t temp = floattoUint32(*(float*)data);
	temp = BIG_LITTLE_SWAP32(temp);
	return _writeValue(client, 1, offset, &temp, 32);
}


//传入json字符串
void* writeData (void *jsonStr){
	char *topic = "plc/putmpinfo";
	paraData* data = NULL;
	int listSize = jsonToParam(jsonStr,&data);
	if(listSize<=0){
		return NULL;
	}

	readMpinfo *infolist = data->dataArray; 
	char * errMsg = NULL;
	for(int i=0;i<listSize;i++){
		S7Object myClient;
		if(connectPLC((void*)&infolist[i],&myClient,0)<0){
			printf("connect to %s error!\n",infolist[i].mpname);
			return NULL;	
		}
		char res = 1;
		switch(infolist[i].valuetype){
			case 1:{
				uint8_t value = strtoul(infolist[i].value, NULL, 10);
				res = writeValue8(&myClient,extractInteger(infolist[i].address),&value);
			}break;
			case 2:{
				uint16_t value = strtoul(infolist[i].value, NULL, 10);
				res = writeValue16(&myClient,extractInteger(infolist[i].address),&value);
			}break;
			case 3:{
				uint32_t value = strtoul(infolist[i].value, NULL, 10);
				res = writeValue32(&myClient,extractInteger(infolist[i].address),&value);
			}break;
			case 4:{
				float value = strtof(infolist[i].value,NULL);
				res = writeValue32f(&myClient,extractInteger(infolist[i].address),&value);
			}break;
			default:
			break;
		}
		disconnect(&myClient);
		if(res < 0){
			errMsg = (char*)malloc(sizeof(char)*201);
			snprintf(errMsg,200,"error:%s_%s failed to write %s",infolist[i].devname,infolist[i].mpname,infolist[i].address);
			break;
		}
	}
	infolist = NULL;
	paramFree(data);
	return errMsg;
}

/**********************************************************************
				功能项
**********************************************************************/
//初始化
void* initPlc(void *data){
	return;
}

float uint32toFloat(uint32_t binary){
	Float32 data;
	data.i=binary;
	return data.f;
}uint32_t floattoUint32(float datafloat){
	Float32 data;
	data.f=datafloat;
	return data.i;
}

//提取地址汇中数字部分
int extractInteger(char *str) {
    int len = strlen(str);
    int i = 0;
    while (i < len && !isdigit(str[i])) {
        i++;
    }

    int start = i; // 开始数字的位置
    while (i < len && isdigit(str[i])) {
        i++;
    }

    char *numStr = (char*)malloc((i - start + 1) * sizeof(char));
    strncpy(numStr, str + start, i - start);
    numStr[i - start] = '\0';

    int result = atoi(numStr); // 将提取的数字部分转换为整数

    free(numStr);
    return result;
}

//获取文件的最后一行，用来返回最新的plc地址中保存的值
void getLastLine(char *filename,char str[]){
	FILE *file = fopen(filename, "r");
	if (file == NULL) {
		perror("Error opening file");
		return;
	}

	fseek(file, 0, SEEK_END);  // 移动文件指针到文件末尾
	long file_size = ftell(file);  // 获取文件大小
	if (file_size == 0) {  // 检查文件是否为空
		printf("File is empty.\n");
		fclose(file);
		return;
	}

	long pos = file_size - 1;
	int ch;
	int line_length = 0;
	char last_line[100];

	// 从文件末尾向前读取，寻找最后一个换行符
	while (pos >= 0) {
		fseek(file, pos, SEEK_SET);
		ch = fgetc(file);
		if (ch == '\n' && pos != file_size - 1) {
			break;
		}
		pos--;
	}

	// 读取最后一行内容
	if (pos < 0) {  // 文件只有一行，无换行符
		fseek(file, 0, SEEK_SET);
	}
	fgets(last_line, sizeof(last_line), file);
	
	//读取的最后一行会有\n，这里要把\n去掉
	int i=0,j=0;
	while (last_line[i]) {
        if (last_line[i] != '\n') {
            str[j++] = last_line[i];
        }
        i++;
    }
    str[j] = '\0'; // 添加字符串结束符
	
	//strcpy(str,last_line);
	//printf("Last line: %s", str);
	fclose(file);
}

//上传功能定时函数
void timer_uploadLog(union sigval sv){
	struct pollNode* tmpNode = (struct pollNode *)sv.sival_ptr;
	pollinfo *info = tmpNode->node;
	
	pthread_mutex_lock(&(tmpNode->mutex));
	
	char *logFile = malloc(sizeof(LOGPATH)+sizeof(info->devname)+sizeof(info->mpname)+7);//以名字命名
	sprintf(logFile,"%s%s_%s.log",LOGPATH,info->devname,info->mpname);
	
//	char *logFile = malloc(sizeof(LOGPATH)+sizeof(info->devid)+sizeof(info->mpid)+7);//============================以id好命名
//	sprintf(logFile,"%s%d_%d.log",LOGPATH,info->devid,info->mpid);
	
//	printf("upload timer\n");
	uploadLog(logFile);	
	free(logFile);
	
	pthread_mutex_unlock(&(tmpNode->mutex));
}

//执行上传功能
int uploadLog(char *filePath){
//	printf("========================upload %s ...\n",filePath);
	if(mqtt_send_file(filePath,NULL) != 0){
		printf("failed to upload file\n");
		return -1;
	}
	
	//上传之后清除对应的文件
	char command[200];
	snprintf(command,200,"rm %s",filePath);
	system(command);
	printf("========================upload %s finished \n",filePath);
    return 0;
}

void listIterator(struct pollInfoList* pollList){
	if(pollList->head == NULL){
		printf("pollList is NULL");
	}
	struct pollNode* debug;
	struct pollNode* tmpNode = pollList->head;
	while(tmpNode!=NULL){
		pthread_mutex_lock(&tmpNode->mutex);
		printf("        listIterator  %s 上锁\n",tmpNode->node->mpname);
		printf("遍历链表：devid:%d,   mpid:%d,   devname:%s,   mpname:%s\n",tmpNode->node->devid,tmpNode->node->mpid,tmpNode->node->devname,tmpNode->node->mpname);
		
		debug = tmpNode;
		
		pthread_mutex_t *tmpMutex = &tmpNode->mutex;
		tmpNode = tmpNode->next;
		pthread_mutex_unlock(tmpMutex);
		printf("        listIterator  %s 解锁\n",debug->node->mpname);
	}
	printf("  listIterator end  \n");
	pthread_mutex_unlock(&pollList->mutex);
	printf("pollList->listSize: %d\n",pollList->listSize);
	pthread_mutex_unlock(&pollList->mutex);
}

struct pollNode* pollNodeInit(pollinfo* nodeInfo){
	struct pollNode* tmp = (struct pollNode*)malloc(sizeof(struct pollNode));
	tmp->next=NULL;
	tmp->pre=NULL;
	tmp->node=nodeInfo;
	pthread_mutex_init(&tmp->mutex,NULL);
	return tmp;
}

struct pollInfoList* pollListInit(){
	struct pollInfoList* tmp = (struct pollInfoList*)malloc(sizeof(struct pollInfoList));
	tmp->head=NULL;
	tmp->rear=NULL;
	tmp->listSize=0;
	pthread_mutex_init(&tmp->mutex,NULL);
	return tmp;
}

struct pollNode* pollListAdd(struct pollInfoList* pollList,pollinfo* nodeInfo){//返回总节点数
	struct pollNode* tmpNode = pollNodeInit(nodeInfo);
	pthread_mutex_lock(&pollList->mutex);
	if(pollList->head==NULL){
		
		pollList->head = tmpNode;
		pollList->rear = tmpNode;
		tmpNode->next = NULL;
		tmpNode->pre = NULL;
	
	}else{
		
		pollList->rear->next = tmpNode;
		tmpNode->pre = pollList->rear;
		pollList->rear = tmpNode;
		
	}
	pollList->listSize++;
	pthread_mutex_unlock(&pollList->mutex);

	return pollList->rear;
}

struct pollNode* pollListDel(struct pollInfoList* pollList,pollinfo* nodeInfo){//返回总节点数
	if(nodeInfo == NULL || pollList->listSize<=0){
		return -1;
	}
	struct pollNode* tmpNode = pollList->head;
	while(tmpNode!=NULL){
		pthread_mutex_lock(&tmpNode->mutex);
		if(tmpNode->node->mpid == nodeInfo->mpid && tmpNode->node->devid == nodeInfo->devid){
			
			if(tmpNode->next==NULL && tmpNode->pre==NULL){//只有一个节点的情况
				
				pthread_mutex_lock(&pollList->mutex);
				pollList->head=NULL;
				pollList->rear=NULL;
				pthread_mutex_unlock(&pollList->mutex);
				
			}else if(tmpNode->next == NULL){//结尾点
				
				pthread_mutex_lock(&tmpNode->pre->mutex);
				tmpNode->pre->next = NULL;
				pollList->rear = tmpNode->pre;
				pthread_mutex_unlock(&tmpNode->pre->mutex);
			
			}else if(tmpNode->pre == NULL){//开头点
				
				pthread_mutex_lock(&tmpNode->next->mutex);
				tmpNode->next->pre = NULL;
				pollList->head = tmpNode->next;
				pthread_mutex_unlock(&tmpNode->next->mutex);
			
			}else{//中间点
				
				pthread_mutex_lock(&tmpNode->next->mutex);
				pthread_mutex_lock(&tmpNode->pre->mutex);
				tmpNode->pre->next = tmpNode->next;
				tmpNode->next->pre = tmpNode->pre;
				pthread_mutex_unlock(&tmpNode->pre->mutex);
				pthread_mutex_unlock(&tmpNode->next->mutex);
				
			}
			pthread_mutex_unlock(&tmpNode->mutex);
			pthread_mutex_destroy(&tmpNode->mutex);
			
			printf("正在free devid=%d...\n",tmpNode->node->devid);
			free(tmpNode);
			printf("		free完成\n");
			break;
		}
		pthread_mutex_unlock(&tmpNode->mutex);
		tmpNode = tmpNode->next;
	}
	
	pthread_mutex_lock(&pollList->mutex);
	pollList->listSize--;
	pthread_mutex_unlock(&pollList->mutex);
	return pollList->head; 
}

struct pollNode* pollListUpdate(struct pollInfoList* pollList,pollinfo* nodeInfo){//返回更新节点的pollNode指针
	if(nodeInfo == NULL || pollList->listSize<=0){
		return -1;
	}
	struct pollNode* tmpNode = pollList->head;
	
	while(tmpNode!=NULL){
		pthread_mutex_lock(&tmpNode->mutex);
		if(tmpNode->node->mpid == nodeInfo->mpid && tmpNode->node->devid == nodeInfo->devid){
			////////////////////
			
//			printf("更新节点前的遍历   pollListUpdate\n");
//			struct pollNode* tmpNode1 = tmpNode;
//			printf("%s_%s:	timer1:%ld  timer2:%ld  uploadTimer:%ld\n",tmpNode1->node->devname,tmpNode1->node->mpname,
//			tmpNode1->node->timer.readTimer1,tmpNode1->node->timer.readTimer2,tmpNode1->node->timer.uploadTimer);


			
			
			////////////////////
			mpTimer timer = tmpNode->node->timer;
			memcpy(tmpNode->node,nodeInfo,sizeof(pollinfo));
			tmpNode->node->timer = timer;
			
			
			////////////////////
			
//			printf("更新节点后的遍历    pollListUpdate\n");
//			tmpNode1 = tmpNode;
//			printf("%s_%s:	timer1:%ld  timer2:%ld  uploadTimer:%ld\n",tmpNode1->node->devname,tmpNode1->node->mpname,
//			tmpNode1->node->timer.readTimer1,tmpNode1->node->timer.readTimer2,tmpNode1->node->timer.uploadTimer);
			
			////////////////////
			pthread_mutex_unlock(&tmpNode->mutex);
			break;
		}
		pthread_mutex_unlock(&tmpNode->mutex);
		tmpNode = tmpNode->next;
	}
	return tmpNode;
}

int freeList(struct pollInfoList* list){//当list为空时，默认情况全局变量pollList，当list不为空的时候清除list
	struct pollNode* tmpNode;
	struct pollInfoList* tmpList;
	if(list == NULL){
		if(pollList==NULL){
			return 0;
		}
		tmpNode = pollList->head;
		tmpList = pollList;
	}else{
		tmpNode = list->head;
		tmpList = list;
	}

	while(tmpNode!=NULL){
		pollListDel(tmpList,tmpNode->node);
		tmpNode = tmpList->head;
	}
	
	pthread_mutex_lock(&tmpList->mutex);
	tmpList->listSize = 0;
	pthread_mutex_unlock(&tmpList->mutex);
	pthread_mutex_destroy(&tmpList->mutex);
	free(tmpList);
	tmpList = NULL;
	return 0; 
}

struct pollNode* selectNodefromList(struct pollInfoList* pollList,pollinfo* nodeInfo){
	if(nodeInfo == NULL || pollList->listSize<=0){
		return -1;
	}
	struct pollNode* tmpNode = pollList->head;
	while(tmpNode!=NULL){
		pthread_mutex_lock(&tmpNode->mutex);
		if(tmpNode->node->mpid == nodeInfo->mpid && tmpNode->node->devid == nodeInfo->devid){
			pthread_mutex_unlock(&tmpNode->mutex);
			return tmpNode;
		}
		pthread_mutex_unlock(&tmpNode->mutex);
		tmpNode = tmpNode->next;
	}
}




