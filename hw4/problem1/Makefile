#LINUXDIR := /home/marko/installs/buildroot/output/build/linux-4796173fc58688055a99a1cef19a839174067220
#obj-m := pthread.o

CC = gcc
CFLAGS = -g -Wall -lrt -pthread

TARGET = pthread

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET) *.o

#all: 
#make ARCH=arm CROSS_COMPILE=arm-buildroot-linux-uclibcgnueabihf- -C $(LINUXDIR) M=$(shell pwd)

#clean: 
#	make -C $(LINUXDIR) M=$(shell pwd) clean
#$(RM) $(TARGET) *.o .*.dwo *.d


