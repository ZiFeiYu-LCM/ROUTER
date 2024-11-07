#ifndef MQTT_H
#define MQTT_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include "MQTTClient.h"
#include "tojson.h"
#include "MQTTAsync.h"

#include "misc/moddate.h"
//#include "net/httpserver.h"
#include "plc/plcctl.h"
#include "net/setip.h"
#include "net/dhcp.h"
#include "net/route.h"
#include "net/ssh.h"
#include "lcm/test.h"
#include "net/firewall.h"
#include "tojson.h"
#include "web/webdatabase.h"
//#include "net/request.h"



typedef struct thread_param{
	char *topic;
	char *jsonStr;
	int len;
}thParam;

//这里的订阅主题，ip，端口，setMqttOps(20,1,"lcm","sa");和连接配置信息后期要重配置文件中获取;

static MQTTAsync client;
static char serverIP[50];
static char clientID[40];
#define QOS         1
#define TIMEOUT     10000L


void connlost(void *context, char *cause);
MQTTAsync_connectOptions setMqttOps(int aliveInterval,int cleanFlag,char* usr,char*pw);
int msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message);
void delivered(void *context, MQTTAsync_deliveryComplete dt);


int mqtt_send(char* str,char *topic);
int readConf();
void* mqtt_main(void *func);
void* mqtt_stop();
void mqtt_pass_exitFlag(char* exitFlag);
int mqtt_send_file(char* file_path,char *topic);
void mqtt_send_error(char* res,char *topic);
#endif