#include "setip.h"

char* setIpAddr(void *arg){	
	char *topic = "net/setipinfo";
	int resCode = 1;
	char *res = NULL;
	paraData* data = NULL;
	int tmpNum = jsonToParam(arg,&data);
	if(tmpNum!=1){
		return "error:params error!";
	}
	ipinfo *infoArray = (ipinfo*)data->dataArray;
	ipinfo *info = &infoArray[0];

    char new_mac[6];
    printf("ifname:%s,addr:%s,mask:%s,gateway:%s,mac:%s\n",info->ifname,info->ipaddr,info->mask,info->gateway,info->mac_str);
    int rc;
    struct ifreq ifr; 
    struct sockaddr_in *sin;
    struct rtentry  rt;
	char cmd[200];
	
	//查找是否有选定的网口名
	snprintf(cmd,200,"ls /sys/class/net/",NULL);
	FILE *fp = popen(cmd,"r");
	if(fp==NULL){
		return "error:failed to read interface";
	}
	char line[MAXLENGTH];
	fgets(line,MAXLENGTH,fp);
	pclose(fp);
	printf("line=%s strlen(line)=%d strlen(info->ifname)=%d\n",line,strlen(line),strlen(info->ifname));
	if(strlen(info->ifname)<=0 || strlen(line)<=0 || !strstr(line,info->ifname)){
		return "error:failed to find selected interface";
	}
	
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if(fd < 0)
    {
		perror("socket error");     
		resCode = -1;     
    }
    //memset(&ifr,0,sizeof(ifr)); 
    strncpy(ifr.ifr_name, (const char *)info->ifname, IFNAMSIZ-1); 
    sin = (struct sockaddr_in*)&ifr.ifr_addr;     
    sin->sin_family = AF_INET;     
    //IP地址
	if(strlen(info->ipaddr)>0){
		if(inet_aton(info->ipaddr,&(sin->sin_addr)) < 0)   
		{     
			perror("inet_aton error");   
			close(fd);  
			return "error:failed to set ip";    
		}    
	 
		if(ioctl(fd,SIOCSIFADDR,&ifr) < 0)   
		{     
			perror("ioctl SIOCSIFADDR error");
			close(fd);     
			return "error:failed to set ip";
		}
	}
    //子网掩码
	if(strlen(info->mask)>0){
		if(inet_aton(info->mask,&(sin->sin_addr)) < 0)   
		{     
			perror("inet_pton error");     
			close(fd);
			resCode = -4;     
		}    
		if(ioctl(fd, SIOCSIFNETMASK, &ifr) < 0)
		{
			perror("ioctl(SIOCSIFNETMASK) error in netmask");
			close(fd);
			return "error:failed to set mask";
		}
	}
    //网关
	if(strlen(info->gateway)>0){
		memset(&rt, 0, sizeof(struct rtentry));
		memset(sin, 0, sizeof(struct sockaddr_in));
		sin->sin_family = AF_INET;
		sin->sin_port = 0;
		if(inet_aton(info->gateway, &sin->sin_addr)<0)
		{
		   printf ( "inet_aton error\n" );
		   close(fd);
		   return "error:failed to set gateway";
		}
		memcpy ( &rt.rt_gateway, sin, sizeof(struct sockaddr_in));
		((struct sockaddr_in *)&rt.rt_dst)->sin_family=AF_INET;
		((struct sockaddr_in *)&rt.rt_genmask)->sin_family=AF_INET;
		rt.rt_flags = RTF_GATEWAY;
		if (ioctl(fd, SIOCADDRT, &rt)<0)
		{
			perror( "ioctl(SIOCADDRT) error in set_default_route");
			close(fd);
			return "error:failed to set gateway";
		}
	}
    //修改MAC地址
    //解析MAC地址字符串并存储到new_mac数组中
	if(strlen(info->mac_str)>0){
		sscanf(info->mac_str, "%02x:%02x:%02x:%02x:%02x:%02x",&new_mac[0],&new_mac[1],&new_mac[2],&new_mac[3],&new_mac[4],&new_mac[5]);
		//printf("mac = %02x:%02x:%02x:%02x:%02x:%02x\n",new_mac[0],new_mac[1],new_mac[2],new_mac[3],new_mac[4],new_mac[5]);
		
		ifr.ifr_addr.sa_family = ARPHRD_ETHER;
		memcpy(ifr.ifr_hwaddr.sa_data, new_mac, 6);
		// 修改MAC地址
		if (ioctl(fd, SIOCSIFHWADDR, &ifr) == -1) {
			perror("SIOCSIFHWADDR");
			close(fd);
			return "error:failed to set mac";
		}
	}
    close(fd);
    return NULL;
}


/*
 * 	ip -o link show | awk '{print $2, $(NF-2)}'//获得网口，mac
	ip link show eth0 | awk '/ether/ {print $2}'//指定网口  获得mac
	ip -o -4 addr show eth0 | awk '{print $4}'//指定网口  获得ip，子网掩码
	ip route show default | grep 'eth0' | awk 'NR==1 {print $3}'//指定网口  获得网关
	
 * */
char* get_ip_address(void *arg){
	char *topic = "net/getipinfo"; 
	char *res = NULL;
	char cmd[100];
	char line[MAXLENGTH];
	FILE *fp=NULL;
	int index = 0, number=0;
	//获取一共多少个网口
	snprintf(cmd,100,"ip -o link show | awk '{print $2, $(NF-2)}'",NULL);
	fp = popen(cmd,"r");
	if(fp==NULL){
		return "error:failed to get net info";
	}
	while(fgets(line,MAXLENGTH,fp)!=NULL){
		if(strstr(line,"lo:")){
			memset(line,0,MAXLENGTH);
			continue;
		}
		number++;
		memset(line,0,MAXLENGTH);
	}
	pclose(fp);
	ipinfo *ipinfoArray = (ipinfo*)malloc(sizeof(ipinfo)*number);
	
	//将网口名和mac填入结构体
	fp = popen(cmd,"r");
	if(fp==NULL){
		return "error:failed to get net info";
	}
	index = 0;
	while(fgets(line,MAXLENGTH,fp)!=NULL){
		if(strstr(line,"lo:")){
			memset(line,0,MAXLENGTH);
			continue;
		}
		printf("%s\n", line);
		sscanf(line,"%[^:]:%17s",ipinfoArray[index].ifname,ipinfoArray[index].mac_str);
		index++;
		memset(line,0,MAXLENGTH);
	}
	pclose(fp);
	//获取ip  子网掩码
	for(int i=0;i<number;i++){
		snprintf(cmd,100,"ip -o -4 addr show %s | awk '{print $4}'",ipinfoArray[i].ifname);
		fp = popen(cmd,"r");
		if(fp==NULL){
			return "error:failed to get net info";
		}
		fgets(line,MAXLENGTH,fp);
		
		char *ptr = strtok(line,"/");
		strcpy(ipinfoArray[i].ipaddr,ptr);
		
		ptr = strtok(NULL,"/");
		mask_to_str(cidr_to_mask(atoi(ptr)),&ipinfoArray[i].mask);
		
		memset(line,0,MAXLENGTH);
		pclose(fp);
	}
	//指定网口  获得网关
	for(int i=0;i<number;i++){
		snprintf(cmd,100,"ip route show default | grep 'eth0' | awk 'NR==1 {print $3}'",ipinfoArray[i].ifname);
		fp = popen(cmd,"r");
		if(fp==NULL){
			return "error:failed to get net info";
		}
		fgets(line,MAXLENGTH,fp);
		
		strcpy(ipinfoArray[i].gateway,line);

		memset(line,0,MAXLENGTH);
		pclose(fp);
	}
	paraData *data = paramInit(topic,number,200);
	data->dataArray = ipinfoArray;
	res = paramToJson(data);
	paramFree(data);
	return res;
}



uint32_t cidr_to_mask(int cidr) {
    return cidr == 0 ? 0 : ~((1 << (32 - cidr)) - 1);
}

// 将32位子网掩码转换为点分十进制格式
void mask_to_str(uint32_t mask, char *buf) {
    sprintf(buf, "%d.%d.%d.%d", 
            (mask >> 24) & 0xFF, 
            (mask >> 16) & 0xFF, 
            (mask >> 8) & 0xFF, 
            mask & 0xFF);
}