CC = gcc

obj-m := aes_encrypt.o

KDIR := /lib/modules/`uname -r`/build

all:
	make -C $(KDIR) M=`pwd` modules

test: test.c
	${CC} -o test test.c

clean:
	make -C $(KDIR) M=`pwd` clean

install:
	sudo dmesg -C
	sudo insmod aes_encrypt.ko

remove:
	sudo rmmod aes_encrypt