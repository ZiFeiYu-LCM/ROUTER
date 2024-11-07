#ifndef CMDS_H
#define CMDS_H
#include "threadpool.h"
#include "tojson.h"
#include "mqtt.h"
char exitFlag = 1;//主程序退出标志，分为3部，1是开启本地http，2是关闭http，最后是退出
/*
#define true 1
#define false 0
#define EXITSER 999


#define SETTIMECMD 101
#define GETTIMECMD 102
#define SETTIMEZONE 103
#define STARTNTP 104
#define STOPNTP 105
#define GETNTPLIST 106
#define ADDNTPERVER 107
#define DELNTPSERVER 108
//#define HTTPRUN 201
//#define HTTPSTOP 202
#define INITPLC 300
#define PLCRUN 301
#define PLCSTOP 302
#define WPLC 303
#define GETMPINFO 304
#define SELECTMP 305
//#define SELECTMP 306

#define SETSTATICIP 401
#define GETIP 402

#define DHCPSTART 501
#define DHCPSTOP 502
#define DHCPRANGE 503
#define ROUTSHOW 504  //显示路由
#define ADDROUTE 505  //添加路由
#define STATICROUTESHOW 506  //显示静态路由
#define HOSTSHOW 507  //主机列表
extern char udhcpdFlag;
char exitFlag = 1;//主程序退出标志，分为3部，1是开启本地http，2是关闭http，最后是退出
//char *jsonStr=NULL;
paraData *paramData=NULL;
threadpool_t *thp;//线程池，用来添加任务


char *runcmd(int cmdValue,paraData *data){
	switch(cmdValue){
		case 999:
			exitFlag = 2;
		    break;
		case 111:
		    break;
		case 222:
		    break;
		case GETTIMECMD://102
		    threadpool_add_task(thp, getDate, NULL);
		    break;
		case SETTIMECMD://101
		    threadpool_add_task(thp, setDate, (void*)"2024-05-24 10:30:00");
		    break;
		case SETTIMEZONE://103
		    threadpool_add_task(thp, setTimezone, (void*)"CST");
		    break;
		case STARTNTP://104
		    threadpool_add_task(thp, startNTP, NULL);
		    break;
		case STOPNTP://105
		    threadpool_add_task(thp, stopNTP, NULL);
		    break;
		case GETNTPLIST://106
		    threadpool_add_task(thp, getNTPList, NULL);
		    break;
		case DELNTPSERVER:{//108
		    char *data = (void *)malloc(50*sizeof(char));
		    strcpy(data,"www.baidu.com");
		    threadpool_add_task(thp, delNTPServer, (void*)data);
		    break;}
		case ADDNTPERVER:{//107
		    char *data = (void *)malloc(50*sizeof(char));
		    strcpy(data,"www.baidu.com");
		    threadpool_add_task(thp, addNTPServer, (void*)data);
		    break;}

		//PLCCTL
		case INITPLC:{//300
		    threadpool_add_task(thp, initPlc, NULL);
		    break;}
		case PLCRUN://301  读取plc数据并存储，data为要检测站点列表的json字符串
			if(paramData == NULL){
				printf("paramData is null\n");
				return NULL;
			}
		    threadpool_add_task(thp, pollmp, (void*)paramData);
		    break;
		case PLCSTOP://302
		    threadpool_add_task(thp, pollStop, NULL);
		    break;
		case WPLC:{//303   //case 后面不能直接跟声明语句，不然会报错所以用{}括起来一下   
			writeData((void*)data);
		    break;
		case GETMPINFO://304  从后端数据库请求要检测的站点信息列表，这里resJson为后端传递的 要检测站点列表的json字符串
			
			free(paramData);
			paramData = data;
			
			if(paramData == NULL){
				printf("paramData is null\n");
				return NULL;
			}
			//printf("cmd: %s\n", pollinfoToJson(pollInfoList));
			break;
			
		case SELECTMP:{//305这里是将本地存储的数据传到web页面上,这里将json串传递给http请求
  
			if(paramData == NULL)){
				printf("paramData is null\n");
				return NULL;
			}
			//mqtt_send
			return getMpinfoList(paramData,(char*)data);}
			break;
		case SEND:{
			
			}break;


		case DHCPSTART:{//501
		    if(udhcpdFlag){
			printf("udhcpd is running\n");
			break;
		    }
		    threadpool_add_task(thp, dhcpdstart, NULL);
		    break;}
		case DHCPSTOP:{//502
		    if(!udhcpdFlag){
			printf("udhcpd is not running\n");
			break;
		    }
		    threadpool_add_task(thp, dhcpdstop, NULL);
		    break;}
		case DHCPRANGE:{//503
		    threadpool_add_task(thp, setdhcp, data);
		    break;}

			
		//	net
		case SETSTATICIP:{//401
		    char data[5][200];
		    strcpy(data[0],"eth0");
		    strcpy(data[1],"192.168.3.86");
		    strcpy(data[2],"255.255.255.0");
		    strcpy(data[3],"192.168.3.1");
		    strcpy(data[4],"00:1c:29:33:ca:72");//可以省略
		    threadpool_add_task(thp, setIpAddr, (void*)&data);		
		    break;}	
		case GETIP:{//402
			return get_ip_address((char*)data);
			}break;
			
		case ROUTSHOW:{//504
		    char flag = 0;
		    threadpool_add_task(thp, routeShow, (void *)&flag);
		    break;}
		case ADDROUTE:{//505
		    routeInfo data;
		    strcpy(data.ip,"192.168.3.2");
		    strcpy(data.netmask,"255.255.255.255");
		    strcpy(data.gateway,"192.168.3.100");
		    strcpy(data.interface,"eth0");
		    strcpy(data.metric,"253");
		    //strcpy(data.track,"192.168.3.2");
		    threadpool_add_task(thp, addStaticRoute, (void*)&data);
		    break;}
		case STATICROUTESHOW:{//506
		    threadpool_add_task(thp, staticRouteShow, NULL);
		    break;}
		case HOSTSHOW:{//507
		    threadpool_add_task(thp, hostShow, NULL);
		    break;}

		default:
		    break;
		}
	}
	return NULL;
}
*/
#endif