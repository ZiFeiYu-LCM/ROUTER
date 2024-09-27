#ifndef COMMON_H
#define COMMON_H
#include <stdio.h>
#include <string.h>

#define MAXLENGTH 1024
#define OLD_CONFIG_PATH(x) #x".old"

#define DEVICEFILE "/etc/plc/devlist"
#define FILEDIR "/etc/plc/"
#define WARNFILE "etc/sy//warn"
#define UPLOADURL ""

//启始配置文件
#define LOGPATH "etc/sy/log/"
//#define CONFPATH "etc/sy/syrouter.conf"
#define CONFPATH "/home/lcm/Desktop/myfiles/router/pro/syrouter.conf"
//devname_mpname_warnname.log

//dhcp
#define DHCP_CONF_PATH "/etc/dnsmasq.conf"

//ssh
#define SSH_CONFIG_PATH "/etc/ssh/sshd_config"

//ntp
#define NTP_PATH "/etc/ntp.conf"
//#define NTP_PATH "/home/lcm/Desktop/log/ntp.conf"
//#define OLD_CONFIG_PATH(x) "/home/lcm/Desktop/log/ntp.conf.old"
#define SNTP_PATH "/etc/init.d/ntpdate"



int is_process_running(char *processName);
int cmdRun(char *command);
int delFile(char *path);

#endif
