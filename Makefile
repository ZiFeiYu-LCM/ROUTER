CROSS_COMPILE ?=
ARCH ?=
CFLAGS=-Wall -O0 -shared -g
#定义连接器的选项(-pthread -L/home/lcm/Desktop/libs  指定库文件的目录)
#若定义了ARCH的话则指定库文件的路径，否则使用系统的库
ifdef ARCH
	LIBPATH := -L/home/lcm/Desktop/libs -L/home/lcm/Desktop/libs/exlibs -L/home/lcm/Desktop/libs/usr
	#bin文件所在的目录
	BIN_DIR := ./out/arm
	CFLAGS += -I/home/lcm/Desktop/includes
else
	LIBPATH := -L/home/lcm/Desktop/第三方库/paho.mqtt.c-1.3.13/linux/lib
	BIN_DIR := ./out/linux
	CFLAGS += -I/home/lcm/Desktop/第三方库/paho.mqtt.c-1.3.13/linux/include
endif
  
LDFLAGS ?= $(LIBPATH) -lsnap7 -lssl -lcrypto -levent -levent_openssl -lpthread -lmicrohttpd -lz -lcurl -lcjson -lrt -lpaho-mqtt3a
#设置编译器
CC=${CROSS_COMPILE}gcc
#目标文件所在的目录
OBJS_DIR=objs

#debug文件夹里的makefile文件需要最后执行，所以这里需要执行的子目录要排除debug文件夹，这里使用awk排除了debug文件夹，读取剩下的文件夹
SUBDIRS=$(shell ls -l | grep ^d | awk '{if(($$9 != "${OBJS_DIR}") && ($$9 != "${BIN_DIR}")) print $$9}')
#无需下一行的注释代码，因为我们已经知道debug里的makefile是最后执行的，所以最后直接去debug目录下执行指定的makefile文件就行，具体下面有注释,$9代表第九列，代表文件名。
#记住当前工程的根目录路径
ROOT_DIR=$(shell pwd)
#最终bin文件的名字，可以更改为自己需要的
BIN=main
#获取当前目录下的c文件集，放在变量CUR_SOURCE中
CUR_SOURCE=${wildcard *.c}
#将对应的c文件名转为o文件后放在下面的CUR_OBJS变量中
CUR_OBJS=${patsubst %.c, %.o, $(CUR_SOURCE)}
#将以下变量导出到子shell中，本次相当于导出到子目录下的makefile中
export CC BIN OBJS_DIR BIN_DIR ROOT_DIR CFLAGS LDFLAGS
#注意这里的顺序，需要先执行SUBDIRS最后才能是DEBUG
all:$(SUBDIRS) $(CUR_OBJS) $(BIN)   #DEBUG
#递归执行子目录下的makefile文件，这是递归执行的关键
$(SUBDIRS):ECHO
	make -C $@
#DEBUG:ECHO
#直接去debug目录下执行makefile文
#	make -C main
ECHO:
	@echo $(SUBDIRS)
#将c文件编译为o文件，并放在指定放置目标文件的目录中即OBJS_DIR
$(CUR_OBJS):%.o:%.c
	$(CC) $(CFLAGS) -c $^ -o $(ROOT_DIR)/$(OBJS_DIR)/$@

$(BIN):$(SUBDIRS) $(CUR_OBJS)
	$(CC) -o $(BIN_DIR)/$@ $(OBJS_DIR)/*.o $(LDFLAGS)

clean:
	@rm $(OBJS_DIR)/*.o
#	@rm -rf $(BIN_DIR)/*
