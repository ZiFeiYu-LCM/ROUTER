#include <stdio.h>
#include "main.h"


int main(){
	if(readConf()!=0){
		LOG_ERROR("readConf error");
		return -1;
	}	

start:
	mqtt_pass_exitFlag(&exitFlag);
	exitFlag==1;
	mqtt_main(NULL);

	if(exitFlag==3){//如果不是手动断开连接的，则http_main会直接将exitFlag设置为3，这的话就从start标记处重新连接
		goto start;
	}
	printf("main exit\n");
	return 0;
}

