CC = arm-buildroot-linux-uclibcgnueabihf-gcc
#CFLAGS = -g -Wall -lrt -pthread
CFLAGS = -g -Wall

HEADERS = payload.h

PIPE1 = pipe1_ipc
PIPE2 = pipe2_ipc

TARGETS = $(PIPE1) $(PIPE2) 

all: $(TARGETS)

$(PIPE1): $(PIPE1).c $(HEADERS)
	$(CC) $(CFLAGS) -o $(PIPE1) $(PIPE1).c

$(PIPE2): $(PIPE2).c $(HEADERS)
	$(CC) $(CFLAGS) -o $(PIPE2) $(PIPE2).c

clean:
	$(RM) $(TARGETS) *.o .*.dwo *.d
