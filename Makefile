# CC := gcc
# PROJ_DIR := .
# PROJ_NAME := chat_app
# OUTPUT_PATH := .


# server: server.c
# 	${CC} -o server server.c -pthread

# client: client.c
# 	${CC} -o client client.c -pthread


# clean:
# 	rm -rf server client


KDIR := /lib/modules/`uname -r`/build
all:
	make -C $(KDIR) M=`pwd`
clean:
	make -C $(KDIR) M=`pwd` clean