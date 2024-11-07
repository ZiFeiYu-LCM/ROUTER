#ifndef _PLCCTL_H_
#define _PLCCTL_H_

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <curl/curl.h>
#include <sys/types.h>
#include <sys/stat.h>


#include "plcdatabase.h"
#include "../tojson.h"
#include "../common.h"

#define LINE_LENGTH 1024

//#define DATABASE "/home/lcm/Desktop/workspace/router.db"

typedef struct _pthNode{
	pthread_t id;
	struct _pthNode *next;
}pthNode;

typedef union {
    uint32_t i;
    float f;
}Float32;

typedef struct _warninfo {
	char device[50];
	char station[50];
	char name[50];	
	char condition_type;//1 ==  2 !=  3 >  4 >=  5 <  6 <=
	void *condition_value;
	char warnGrade;
	char txt[1024];
	char label[50];
} warninfo;

struct pollNode {	
	pollinfo* node;
	struct pollNode* pre;
	struct pollNode* next;
	pthread_mutex_t mutex;
};
struct pollInfoList {
	struct pollNode* head;
	struct pollNode* rear;
	int listSize;
	pthread_mutex_t mutex;
};









#define BIG_LITTLE_SWAP16(x)        ( (((*(short int *)&x) & 0xff00) >> 8) | \
                                      (((*(short int *)&x) & 0x00ff) << 8) )
 
#define BIG_LITTLE_SWAP32(x)        ( (((*(long int *)&x) & 0xff000000) >> 24) | \
                                      (((*(long int *)&x) & 0x00ff0000) >> 8) | \
                                      (((*(long int *)&x) & 0x0000ff00) << 8) | \
                                      (((*(long int *)&x) & 0x000000ff) << 24) )
 
#define BIG_LITTLE_SWAP64(x)        ( (((*(long long int *)&x) & 0xff00000000000000) >> 56) | \
                                      (((*(long long int *)&x) & 0x00ff000000000000) >> 40) | \
                                      (((*(long long int *)&x) & 0x0000ff0000000000) >> 24) | \
                                      (((*(long long int *)&x) & 0x000000ff00000000) >> 8) | \
                                      (((*(long long int *)&x) & 0x00000000ff000000) << 8) | \
                                      (((*(long long int *)&x) & 0x0000000000ff0000) << 24) | \
                                      (((*(long long int *)&x) & 0x000000000000ff00) << 40) | \
                                      (((*(long long int *)&x) & 0x00000000000000ff) << 56) )


#define rackNumber 0 //机架号
#define slotNumber 1 //插槽号



//数据库操作
struct mysqlStruct* con_routerbase();
void close_routerbase(struct mysqlStruct* mysqldb);
struct mysql_res_struct select_from_database(struct mysqlStruct* mysqldb);









//线程列表
int traverseList(pthNode *pthHead,pthNode *pthCur);
int listFree(pthNode *pthHead,pthNode *pthCur);
int addPthNode(pthNode **pthCur,pthread_t id);
int extractInteger(char *str);
//读写
uint32_t floattoUint32(float datafloat);
float uint32toFloat(uint32_t binary);
void *writeData(void *jsonStr);
int readData(pollinfo *info,void *data);


void* initPlc(void *data);
void getLastLine(char *filename,char str[]);
char checkWarn(char conditionL,char conditionM,char conditionR,float value1,float value2,float value3);//查看是否报警

//定时器
void timer_handler(union sigval sv);
void timer_uploadLog(union sigval sv);
int uploadLog(char *filePath);

//cmd调用，运行线程函数
void *pollmp(void* arg);
int _pollmp(void* arg);//线程函数，开启定时器
//cmd调用，停止
void *pollStop(void* arg);


//功能函数
void *getMpinfoList(char *jsonStr);
void *getMpinfoList_update(char *jsonStr);
void *getMpinfoList_delete(char *jsonStr);
void *getMpinfoList_add(char *jsonStr);
char *putMpinfoList(char *jsonStr);//put data to web
int timerOpsSet(struct pollNode* nodeinfo);
void *freeMpinfoArray(char *arg);

//链表函数
struct pollNode* pollNodeInit(pollinfo* nodeInfo);
struct pollInfoList* pollListInit();
struct pollNode* pollListAdd(struct pollInfoList* pollList,pollinfo* nodeInfo);
struct pollNode* pollListDel(struct pollInfoList* pollList,pollinfo* nodeInfo);
struct pollNode* pollListUpdate(struct pollInfoList* pollList,pollinfo* nodeInfo);
int freeList(struct pollInfoList* pollList);
void listIterator(struct pollInfoList* pollList);
struct pollNode* selectNodefromList(struct pollInfoList* pollList,pollinfo* nodeInfo);//返回要查询的节点指针


//用来存储后端返回要检测的mp信息列表
pollinfo *pollInfoArray;
int pollInfoArraySize;
struct pollInfoList* pollList;

//数据库
extern struct mysqlStruct* routerBase;


pthread_mutex_t connetctMutex;
#endif
