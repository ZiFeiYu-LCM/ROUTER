#include "mqtt.h"

int mqtt_Stop_Flag = 1;
//funcPtr runCmd;
char *mainExitFlag = NULL;//主程序退出标志
LookupTable lookupTable[] = {
	{"time/gettime",getDate},					//获取时间
	{"time/settime",setDate},					//获取时间
	{"time/setTimezone",setTimezone},			//时区域

	{"net/setipinfo",setIpAddr}, 				//设置IP地址
	{"net/getipinfo",get_ip_address},			//获取IP地址
	{"net/startssh",startssh}, 					//开启ssh
	{"net/stopssh",stopssh},	  				//关闭ssh
	{"net/setssh",setssh},	  					//设置ssh

	{"time/startntp",startNTP},					//开启时间服务器
	{"time/stopntp",stopNTP},					//关闭时间服务器
	{"time/getntpinfo",getNTPinfo},				//获取时间服务器
	{"time/setntpinfo",setNTPinfo},				//设置时间服务器
	{"time/delntpserver",delNTPServer},			//删除时间服务器

	{"dhcp/setdhcpinfo",setiprange},			//设置dhcp信息
	{"dhcp/dhcpstart",dhcpstart},				//开启dhcp
	{"dhcp/dhcpstop",dhcpstop},					//关闭dhcp
	{"dhcp/getdhcpinfo",getdhcpinfo},			//获取dhcp信息

//	{"router/showrouter",routeShow}
	{"router/showhost",hostShow},				//显示局域网内主机
//	{"router/showstaticrouter",staticRouteShow}
//	{"router/addstaticrouter",addStaticRoute}

	{"net/addFireRule",addFirewallRule},		//添加防火墙规则
	{"net/getFireRule",getFirewallRule},		//获取防火墙规则
	{"net/delFireRule",delFirewallRule},		//删除防火墙规则

	{"plc/getmpinfo",getMpinfoList},				//获取plc信息
	{"plc/getmpinfo/update",getMpinfoList_update},	//更新本地的站点信息
	{"plc/getmpinfo/delete",getMpinfoList_delete},	//删除本地的站点信息
	{"plc/getmpinfo/add",getMpinfoList_add},		//获取站点信息
	{"plc/putmpinfo",putMpinfoList},				//将站点信息返回到前端
	{"plc/pollmp",pollmp},							//开始站点检测
	//{"plc/writedata",writeData},
	{"plc/pollstop",pollStop},						//停止站点检测
	{"plc/clean",freeList},							//释放本地站点
	
	{"exit",mqtt_stop},								//退出
	
	
/********************** web **********************/
	{"web/pull_dev_info",web_pull_plcinfo},
	{"web/pull_dev_info_allnum",web_pull_plcinfo_num},
	{"web/pull_mp_info",web_pull_mpinfo},
	{"web/pull_mp_info_allnum",web_pull_mpinfo_num},
	{"web/add_mp_info",web_add_mpinfo},
	{"web/update_mp_info",web_update_mpinfo},
/********************** web  end**********************/
};

//读取配置文件
int readConf(){
	FILE *fp = fopen(CONFPATH,"r");
	if(fp==NULL){
		LOG_ERROR("failed to open conifg file");
		return -1;
	}
	char line[100];
	while(fgets(line,sizeof(line),fp) ){
		if(line[0] == '#')
			continue;
		char *keystr = strtok(line,":");
		char *valuestr = strtok(NULL,"\n");
		if(strcmp(keystr,"ip")==0){
			snprintf(serverIP,50,"%s",valuestr);
		}else if(strcmp(keystr,"port")==0){
			char ip[20];
			strcpy(ip,serverIP);
			snprintf(serverIP,50,"%s:%s",ip,valuestr);
		}else if(strcmp(keystr, "clientid")==0){
			snprintf(clientID,40,"%s",valuestr);
		}
		memset(line,0,100);
	}
	printf("ip:%s, id:%s\n",serverIP,clientID);
	fclose(fp);
	
	int size = sizeof(lookupTable)/sizeof(lookupTable[0]);
	for(int i=0;i<size;i++){
		char tmpkey[51];
		strncpy(tmpkey,lookupTable[i].key,50);
		snprintf(lookupTable[i].key,100,"%s/%s",clientID,tmpkey);
	}
	
	return 0;
}

void mqtt_send_error(char* res,char *topic){
	paraData *data = paramInit("error",1,-200);
	data->dataArray = malloc( sizeof(char)*(strlen(res)+1));
	strcpy(data->dataArray,res);
	res = paramToJson(data);
	mqtt_send(res,topic);
	if(data!=NULL){
		paramFree(data);
	}
}

//MQTT接收到消息后的线程处理了函数
void* deal(void *param){
	thParam *param_data = (thParam*)param;
	printf("msg:%s  topic:%s  len=%d\n",param_data->jsonStr,param_data->topic,param_data->len);

	char *tmpStr = NULL;
	if(param_data->len > 0){
		tmpStr = (char*)malloc((param_data->len+1)*sizeof(char));
		memset(tmpStr,param_data->len+1,0);
		strcpy(tmpStr,param_data->jsonStr);
	}
	int size = sizeof(lookupTable)/sizeof(lookupTable[0]);
	char *res=NULL;//这里res可能返回NULL：表示正常执行完成，返回字符串：表示要返回，返回error：表示执行错误
	char isexit = 1;
	for(int i=0;i<size;i++){
		if(strcmp(lookupTable[i].key,param_data->topic)==0 ){
			res = lookupTable[i].function(tmpStr);
			isexit = 0;
		}
	}

	if(isexit){
		char errmsg[] = "error:No matching function";
		res = errmsg;
	}
	paraData* data = NULL;
	if(res == NULL){//表示运行正常
		//运行正常
		
		data = paramInit(NULL,0,200);
		res = paramToJson(data);
	}else if(strncmp(res,"error:",strlen("error:"))==0){//表示返回错误
		data = paramInit("error",0,-200);
		data->dataArray = malloc( sizeof(char)*(strlen(res)+1));
		strcpy(data->dataArray,res);
		res = paramToJson(data);
	}
	
	//返回数据 返回的数据会在原有的topic后加一个/response
	char tmp_topic[100];
	snprintf(tmp_topic,100,"%s/response",param_data->topic);
	mqtt_send(res,tmp_topic);
	
	
	
	//清除malloc分配的内存
    if(data!=NULL){
		paramFree(data);
	}
	if(res!=NULL){
		free(res);
	}
	if(tmpStr!=NULL)
		free(tmpStr);
	
	if(param_data->topic != NULL)
		free(param_data->topic);
	if(param_data->jsonStr != NULL)
		free(param_data->jsonStr);
	free(param_data);
	return 1; //这里必须返回1，来告诉mqtt服务器接收成功，服务器会一直发消息	
}

void delivered(void *context, MQTTAsync_deliveryComplete dt) {//信息已经被其他订阅者接收到
    //printf("Message with token value %d delivery confirmed\n", dt);
}

void onSend(void *context, MQTTAsync_successData *response) {//信息已经被发送出去,服务器接收到
    printf("Message with token value %d delivery confirmed\n", response->token);
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message) {//有信息到达时运行，异步mqtt是可以多线程运行的
    
	if(strstr(topicName,"/response")){//过滤掉自己的回复
		MQTTAsync_freeMessage(&message);
		return 1;
	}
	// printf("Message arrived\n");
    printf("     topic: %s\n", topicName);
    printf("   message: %.*s\n", message->payloadlen, (char*)message->payload);
	
	//const char *str_end = topicName + strlen(topicName) - strlen("/response");
	//return 1;
	
	thParam *data = (thParam*)malloc(sizeof(thParam));
	
	data->len = message->payloadlen;
	
	data->topic = malloc(sizeof(char)*(strlen(topicName)+2));
	memset(data->topic,strlen(topicName)+2,0);
	strcpy(data->topic,topicName);
	
	if(message->payloadlen == 0){
		data->jsonStr = NULL;
	}else{
		data->jsonStr = malloc(sizeof(char)*(message->payloadlen+2));
		memset(data->jsonStr,message->payloadlen+2,0);
		strcpy(data->jsonStr,(char*)message->payload);
	}
	
	//线程结束后不会自动释放占用的资源要使用pthread_detach 或 pthread_join来释放
	pthread_t thread;
	if (pthread_create(&thread, NULL, deal, (void*)data) != 0) {
        printf("Failed to create thread, topic %s\n",topicName);
    }else{
		//让线程结束后自动释放
		pthread_detach(thread);
	}
	// printf("msgarrvd over\n");
	MQTTAsync_freeMessage(&message);
	return 1;//这里必须返回1，来告诉mqtt服务器接收成功，服务器会一直发消息
}

void onConnectFailure(void *context, MQTTAsync_failureData *response) {//连接失败
    printf("Connect failed, rc %d\n", response ? response->code : 0);
    exit(EXIT_FAILURE);
}

void connlost(void *context, char *cause) {//已经建立连接，但是连接丢失
    printf("Connection lost\n");
	if(*mainExitFlag != 2){
		printf("     cause: %s\n", cause);
		printf("     tring to restart...\n");
		mqtt_Stop_Flag = 1;
	}
}

void onConnect(void *context, MQTTAsync_successData *response) {//连接成功时，订阅主题
    printf("Connected\n");
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    int rc;

    opts.onSuccess = NULL;
    opts.onFailure = NULL;
    opts.context = client;

	if(strlen(clientID)==0){
		printf("clientID is null!\n");
		mqtt_stop();
		return;
	}
	
	char topic[50];//订阅主题
	snprintf(topic,50,"%s/#",clientID);
	if ((rc = MQTTAsync_subscribe(client, topic, QOS, &opts)) != MQTTASYNC_SUCCESS) {
		printf("Failed to subscribe to topic %s, return code %d\n", topic, rc);
	}
	
	mqtt_send(clientID,"public/online");//向public主题发布消息
	
	/*size_t lookupTableSize = sizeof(lookupTable) / sizeof(lookupTable[0]);
	for(int i=0;i<lookupTableSize;i++){
		if ((rc = MQTTAsync_subscribe(client, lookupTable[i].key, QOS, &opts)) != MQTTASYNC_SUCCESS) {
			printf("Failed to subscribe to topic %s, return code %d\n", lookupTable[i].key, rc);
		}
	}*/
}

void onDisconnect(void *context, MQTTAsync_successData *response) {//手动断开连接
    printf("Disconnected\n");
}



int mqtt_send(char* str,char *topic){
	int rc;
	MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
	MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
   // MQTTAsync_deliveryToken token;
	// Publish a message
    pubmsg.payload = str;
    pubmsg.payloadlen = (int)strlen(str);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
	opts.onSuccess = onSend;
	opts.context = client;
	if ((rc = MQTTAsync_sendMessage(client, topic, &pubmsg, &opts)) != MQTTASYNC_SUCCESS) {
		printf("Failed to start sendMessage, return code %d\n", rc);
		return -1;
	}
	return 0;
}

int mqtt_send_file(char* file_path,char *topic){
	//printf("start file pass, file_path:%s \n",file_path);
	
	int bufSize = 1024;
	FILE *file = fopen(file_path, "rb");
    if (!file) {
        perror("Failed to open file");
        return -1;
    }
	MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    char buffer[bufSize];
    size_t bytes_read;
	
	opts.onSuccess = onSend;
    opts.context = client;

	//发送文件名
	
	//将文件名提取出来
	char *fileName = file_path + strlen(file_path);
	if(!strstr(file_path,"/")){
		fileName = file_path;
		printf("1 fileName = %s\n",fileName);
	}else{
		while((char)*fileName != '/'){
			fileName--;
		}
		fileName++;
		printf("2 fileName = %s\n",fileName);
	}
	
	//发送文件启始 标志
	snprintf(buffer,bufSize,"file_pass_start:%s",fileName);
	pubmsg.payload = buffer;
	pubmsg.payloadlen = (int)strlen(buffer);
	pubmsg.qos = QOS;
	pubmsg.retained = 0;
	int rc = MQTTAsync_sendMessage(client, topic, &pubmsg, &opts);
	if (rc != MQTTASYNC_SUCCESS) {
		printf("Failed to send message, return code %d\n", rc);
		return -1;
	}
	// 暂停一下等待消息发送完成
	usleep(100000); // 100毫秒
	memset(buffer,0,bufSize);
	
	//发送文件
    while ((bytes_read = fread(buffer, 1, bufSize, file)) > 0) {
        pubmsg.payload = buffer;
        pubmsg.payloadlen = (int)bytes_read;
        pubmsg.qos = QOS;
        pubmsg.retained = 0;

        int rc = MQTTAsync_sendMessage(client, topic, &pubmsg, &opts);
        if (rc != MQTTASYNC_SUCCESS) {
            printf("Failed to send message, return code %d\n", rc);
			mqtt_send_error("error:failed to send file context",topic);
			return -1;
        }
        // 暂停一下等待消息发送完成
        usleep(100000);// 100毫秒
		memset(buffer,0,bufSize);
    }

    fclose(file);
	
	//传输结束标志
	snprintf(buffer,bufSize,"file_pass_over:%s",fileName);
	pubmsg.payload = buffer;
	pubmsg.payloadlen = (int)strlen(buffer);
	pubmsg.qos = QOS;
	pubmsg.retained = 0;
	rc = MQTTAsync_sendMessage(client, topic, &pubmsg, &opts);
	if (rc != MQTTASYNC_SUCCESS) {
		printf("Failed to send message, return code %d\n", rc);
		mqtt_send_error("error:failed to send file_pass_over",topic);
		return -1;
	}

	return 0;
}

MQTTAsync_connectOptions setMqttOps(int aliveInterval,int cleanFlag,char* usr,char*pw){
	MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
	conn_opts.keepAliveInterval = aliveInterval;
    conn_opts.cleansession = cleanFlag;
	if(usr != NULL && pw != NULL){
		conn_opts.username = usr;  // 设置用户名
		conn_opts.password = pw;  // 设置密码
	}
	conn_opts.onSuccess = onConnect;
	conn_opts.onFailure = onConnectFailure;
	conn_opts.context = client;
	return conn_opts;
}

void* mqtt_main(void *func){
	if(mainExitFlag == NULL){
		printf("mainExitFlag is NULL\n");
		return NULL;
	}
	if(!mqtt_Stop_Flag){
		printf("mqtt is running\n");
		return NULL;
	}

	int rc;
	//client是全局变量
	//MQTTAsync_connectOptions conn_opts = setMqttOps(20,0,"lcm","sa");
	MQTTAsync_connectOptions conn_opts = setMqttOps(20,0,NULL,NULL);
	//遗嘱消息
	MQTTAsync_willOptions will_opts = MQTTAsync_willOptions_initializer;
    will_opts.message = "test exit";
    will_opts.qos = QOS;
    will_opts.topicName = "public/offline";
    will_opts.retained = 0;  // 不保留消息
	conn_opts.will = &will_opts;
	
	printf("serverIP = %s\n",serverIP);  //tcp://192.168.3.20:1883
	MQTTAsync_create(&client, serverIP, clientID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
	MQTTAsync_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
	if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS) {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }

	mqtt_Stop_Flag = 0;
	while(!mqtt_Stop_Flag);
    MQTTAsync_disconnectOptions disc_opts = MQTTAsync_disconnectOptions_initializer;
    disc_opts.onSuccess = onDisconnect;
    MQTTAsync_disconnect(client, &disc_opts);
    MQTTAsync_destroy(&client);
	if(mainExitFlag != NULL){
		*mainExitFlag = 3;
	}
}

void* mqtt_stop(){
	
	if(mainExitFlag == NULL){
		printf("mainExitFlag is NULL\n");
		return NULL;
	}
	pollStop(NULL);
	sleep(1);
	freeList(NULL);
	mqtt_Stop_Flag = 1;
    *mainExitFlag = 2;//告诉主程序http已经停止，可以继续停止后面的程序。
	mainExitFlag = NULL;
	return NULL;
}

void mqtt_pass_exitFlag(char* exitFlag){
	mainExitFlag = exitFlag;
}


