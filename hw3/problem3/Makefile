LINUXDIR := /home/marko/installs/buildroot/output/build/linux-4796173fc58688055a99a1cef19a839174067220

obj-m := name_timer.o

all: 
	make ARCH=arm CROSS_COMPILE=arm-buildroot-linux-uclibcgnueabihf- -C $(LINUXDIR) M=$(shell pwd)
#make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) modules

clean: 
	make -C $(LINUXDIR) M=$(shell pwd) clean
#make -C /lib/modules/$(shell uname -r)/build M=$(shell pwd) clean
