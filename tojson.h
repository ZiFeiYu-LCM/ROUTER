#ifndef TOJSON_H
#define TOJSON_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cjson/cJSON.h>
#include <pthread.h>
#include <signal.h>

#include "s2j/s2j.h"

#define LOG_ERROR(msg) fprintf(stderr, "Error: In file %s, line %d, function %s, %s\n", __FILE__, __LINE__, __FUNCTION__, msg)
#define REQUEST_ERROR(code, msg) "error:" #code "#" msg " in " __FILE__"!\n"


typedef struct mp_timer{
	timer_t uploadTimer;
	timer_t readTimer1;
	timer_t readTimer2;
	struct sigevent sev1;
	struct itimerspec its1;
	struct sigevent sev2;
	struct itimerspec its2;
	struct sigevent uploadSev;
    struct itimerspec uploadIts;
}mpTimer;


typedef struct _pollinfo{//从后端获取设备测点信息
	//设备
	int   devid;
	char  devname[50] ;
	char  ip[50]	  ;
	char  pollcycle   ;
	char  ismultipoll ;
	char  pollcycle2  ;
	char  istimeout   ;//5000为5000ms
	int   collecttime ;
	int   connecttime ;

	//分组
	char groupname[50];
	int   uploadperiod;
	int   maxitem	  ;

	//测点
	int   mpid;
	char  mpname[50]  ;
	char  addresstype[20];
	char  dbindex     ;
	char  address[20] ;
	char  valuetype   ;
	char  mpnote[200] ;
	//void  *value	  ;//这个参数用来读取和写入时的存储，其他地方用不到
	//报警
	char  warnname[50];
	char  warnlevel[20];	
	int   warncondition;
	float wcvalue1	   ;
	float wcvalue2	   ;
	char  warncontext[200];
	
	mpTimer timer;//定时器任务，传递参数时，传递参数时，可忽略此项

}pollinfo;


typedef struct _readMpinfo {//向前端传递数据
	int mpid;
	int devid;
	char devname[50];
	char mpname[50];
	char groupname[50];
	char valuetype;
	char address[50];
	char mpnote[200];
	char value[50];
	char time[50];
	char ip[50]	 ;
} readMpinfo;


typedef struct _ipinfo{
	char ifname[50];
    char ipaddr[50];
    char mask[50];
    char gateway[50];
    char mac_str[50];//mac地址;
}ipinfo;

typedef struct _dhcpinfo{
	char interface[50];
	char iprange_start[50];
	char iprange_end[50]; 
	int validtime;
}dhcpinfo;

typedef struct _firewallrule{
	int  id;
//	char addordel;
	char table[15];
	char protocol[10];
	char srcip[20];
	char desip[20];
	char port[10];
	char in_interface[10];
	char out_interface[10];
	char jump[10];
}firewallrule;


typedef struct _sshinfo{
	char listenaddr[50];
	char port[10];
	char outtime[20];
}sshinfo;
int jsonToSshinfo(char *jsonStr,sshinfo **data);
char* sshinfoToJson(sshinfo* data,int size);

typedef struct _dnsinfo{
	char domain1[50];
	char domain2[50];
	char ip1[20];
	char ip2[20];
}dnsinfo;
int jsonToDnsinfo(char *jsonStr,dnsinfo **data);
char* dnsinfoToJson(dnsinfo* data,int size);

typedef struct _ntpserver{
	char server[50];
	char ismain;
	char port[10];	
}ntpserver;
typedef struct _ntpinfo{
	void *servers;
	char updatetime[10];
	char interface[10];
	char srcaddr[30];
	char maxpoll;
	char minpoll;
	int serverNum;
}ntpinfo;
int jsonToNtpinfo(char *jsonStr,ntpinfo **data);
char* ntpinfoToJson(ntpinfo* data,int size);

typedef struct _hostinfo{
	char interface[10];
	char mac[50];
	char ip[20];
	char name[30];
	char validtime[20];
}hostinfo;
int jsonToHostinfo(char *jsonStr,hostinfo **data);
char* hostinfoToJson(hostinfo* data,int size);

typedef struct _postParam {//向前端传递数据
	readMpinfo *data;
	int dataSize;
} postParam;


typedef struct _routeInfo{
	char ip[50];
	char netmask[50];
	char gateway[50];
	char interface[50];
	char metric[50];
	char track[50];//linux系统不支持添加track标记，常用于特定的路由器设备或高级路由策略中
	char proto[50];
}routeInfo;



typedef struct _paraData{
	void* dataArray;
	char* msg;
	int dataArrySize;
	int code;
}paraData;


typedef int (*jsonToStruct)(char *jsonStr,void **data);
typedef char* (*structTojson)(void* data,int size);
struct functionTable{
	char *key;
    jsonToStruct toStruct;
	structTojson toJson;
};

int jsonToParam(char *jsonStr,paraData **data);
char* paramToJson(paraData *data);


void paramFree(paraData *data);
paraData* paramInit(char *topic,int dataArrySize,int code);


typedef char* (*funcPtr)(void *data);
typedef struct {
    char key[100];
	funcPtr function;
} LookupTable;



int jsonToFirewallrule(char *jsonStr,firewallrule **data);
char* firewallruleToJson(firewallrule* data,int size);

int jsonToIpinfo(char *jsonStr,ipinfo **data);
char* ipinfoToJson(ipinfo* data,int size);

//传入一个pollinfo字符串数组，data存储一个pollinfo结构体的数组（记得free），返回数组长度
int jsonToPollinfo(char *jsonStr,pollinfo **data);
//传入一个pollinfo结构体的数组，返回一个pollinfo数组字符串，记得free
char* pollinfoToJson(pollinfo* data,int size);


int jsonToReadMpinfo(char *jsonStr,readMpinfo **data);
char* readMpinfoToJson(readMpinfo* data,int size);

//这个的返回值需要free(data->data),free(data);
int jsonToPostparam(char *jsonStr,postParam **data);
char* postparamToJson(postParam* data,int size);


//这个的返回值需要free(data->data),free(data);
int jsonToDhcpinfo(char *jsonStr,dhcpinfo **data);
char* dhcpinfoToJson(dhcpinfo* data,int size);

char* routeinfoToJson(routeInfo* data,int size);


//将一个字符串的参数直接赋值到paraData的dataArray中
int jsonTostr(char *jsonStr,char **str);
char *strToJson(char* str,int size);


/**web  服务的结构体 */
struct dev_info_brief {
	int devid;
	char devname[20];
	char ip[20];
	char note[50];
	char protocol[20];
};
int jsonToDev_info_brief(char *jsonStr,struct dev_info_brief **data);
char* dev_info_briefToJson(struct dev_info_brief* data,int size);


struct mp_info_brief {
	int devid;
	int mpid;
	int groupid;
	char mpname[30];
	char groupname[30];
	char time[30];
	int valuetype;
	char address[30];
	char mpnote[50];

	char addresstype[20];
	int mode;
	int dbindex;
	char value[20];
};
int jsonToMp_info_brief(char *jsonStr,struct mp_info_brief **data);
char* mp_info_briefToJson(struct mp_info_brief* data,int size);



struct mpinfo {
	char mpname[30];
	char addresstype[10];
	int dbindex;
	char address[20];
	int valuetype;
	int mode;
	char mpnote[100];
	int devid;
};
int jsonToMpinfo(char *jsonStr,struct mpinfo **data);
char* mpinfoToJson(struct mpinfo* data,int size);

/**web  服务的结构体  end*/





static struct functionTable functionMap[] = {
    {"plc/getmpinfo", (jsonToStruct)jsonToPollinfo,(structTojson)pollinfoToJson},
	{"plc/getmpinfo/update", (jsonToStruct)jsonToPollinfo,(structTojson)pollinfoToJson},
	{"plc/getmpinfo/delete", (jsonToStruct)jsonToPollinfo,(structTojson)pollinfoToJson},
	{"plc/getmpinfo/add", (jsonToStruct)jsonToPollinfo,(structTojson)pollinfoToJson},
	{"plc/putmpinfo", (jsonToStruct)jsonToReadMpinfo,(structTojson)readMpinfoToJson},
	{"plc/write", (jsonToStruct)jsonToReadMpinfo,(structTojson)readMpinfoToJson},
	
//	{"firewall/setrule", (jsonToStruct)jsonToFirewallrule,(structTojson)firewallruleToJson},
	{"dhcp/getrelay", (jsonToStruct)jsonToDnsinfo,(structTojson)dnsinfoToJson},
	
	{"net/addFireRule", (jsonToStruct)jsonToFirewallrule,(structTojson)firewallruleToJson},
	{"net/delFireRule", (jsonToStruct)jsonToFirewallrule,(structTojson)firewallruleToJson},
	{"net/getFireRule", (jsonToStruct)jsonToFirewallrule,(structTojson)firewallruleToJson},
	
	{"dhcp/getdhcpinfo", (jsonToStruct)jsonToDhcpinfo,(structTojson)dhcpinfoToJson},
	{"dhcp/setdhcpinfo", (jsonToStruct)jsonToDhcpinfo,(structTojson)dhcpinfoToJson},
	
	{"net/getipinfo", (jsonToStruct)jsonToIpinfo,(structTojson)ipinfoToJson},
	{"net/setipinfo", (jsonToStruct)jsonToIpinfo,(structTojson)ipinfoToJson},
	
	{"time/getntpinfo", (jsonToStruct)jsonToNtpinfo,(structTojson)ntpinfoToJson},
	{"time/setntpinfo", (jsonToStruct)jsonToNtpinfo,(structTojson)ntpinfoToJson},
	{"time/delntpserver", (jsonToStruct)jsonToNtpinfo,(structTojson)ntpinfoToJson},
	{"time/getsntpinfo", (jsonToStruct)jsonToNtpinfo,(structTojson)ntpinfoToJson},
	
	{"getstr", (jsonToStruct)jsonTostr,(structTojson)strToJson},//如果参数里返回是一个字符串的话，msg就都用getsrt
	
	
	/*web 的结构体和json互转    */
	{"web/pull_dev_info", (jsonToStruct)jsonToDev_info_brief,(structTojson)dev_info_briefToJson},
	{"web/pull_mp_info", (jsonToStruct)jsonToMp_info_brief,(structTojson)mp_info_briefToJson},
	// {"web/add_mp_info", (jsonToStruct)jsonToMpinfo,(structTojson)mpinfoToJson},
	{"web/add_mp_info", (jsonToStruct)jsonToMp_info_brief,(structTojson)mp_info_briefToJson},
	{"web/update_mp_info", (jsonToStruct)jsonToMp_info_brief,(structTojson)mp_info_briefToJson},
	/*web 的结构体和json互转 end*/
};

#endif









/*
struct _rwinfo{
	//设备
	devname:"plc1",
	ip:"192.168.3.248",
	pollcycle:2,
	ismultipoll:0,
	pollcycle2:0,
	istimeout:0,
	collecttime:5000,
	connecttime:5000,

	//分组
	uploadperiod：500,
	maxitem：500,

	//测点
	mpname："mp1",
	addresstype:"DB",
	dbindex:1,
	address:"V110",
	valuetype:1,

	//报警
	warnname:"warn1",
	warnlevel:"提醒",
	warncondition：406,
	wcvalue1:23.00,
	wcvalue2:32.00,
	warncontext:"this is a  text"
}rwinfo;*/
