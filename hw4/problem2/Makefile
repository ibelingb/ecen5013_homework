CC = arm-buildroot-linux-uclibcgnueabihf-gcc
#CFLAGS = -g -Wall -lrt -pthread
CFLAGS = -g -Wall

HEADERS = payload.h

PIPE1 = pipe1_ipc
PIPE2 = pipe2_ipc
SOCKET1 = socket1_ipc
SOCKET2 = socket2_ipc
SM1 = sm1_ipc
SM2 = sm2_ipc
PQUEUE1 = pqueue1_ipc
PQUEUE2 = pqueue2_ipc

#TARGETS = $(PIPE1) $(PIPE2) $(SOCKET1) $(SOCKET2) $(SM1) $(SM2) $(PQUEUE1) $(PQUEUE2)
TARGETS = $(SOCKET1) $(SOCKET2) 
all: $(TARGETS)

$(PIPE1): $(PIPE1).c $(HEADERS)
	$(CC) $(CFLAGS) -o $(PIPE1) $(PIPE1).c

$(PIPE2): $(PIPE2).c $(HEADERS)
	$(CC) $(CFLAGS) -o $(PIPE2) $(PIPE2).c

$(SOCKET1): $(SOCKET1).c $(HEADERS)
	$(CC) $(CFLAGS) -o $(SOCKET1) $(SOCKET1).c

$(SOCKET2): $(SOCKET2).c $(HEADERS)
	$(CC) $(CFLAGS) -o $(SOCKET2) $(SOCKET2).c

$(SM1): $(SM1).c $(HEADERS)
	$(CC) $(CFLAGS) -o $(SM1) $(SM1).c

$(SM2): $(SM2).c $(HEADERS)
	$(CC) $(CFLAGS) -o $(SM2) $(SM2).c

$(PQUEUE1): $(PQUEUE1).c $(HEADERS)
	$(CC) $(CFLAGS) -o $(PQUEUE1) $(PQUEUE1).c

$(PQUEUE2): $(PQUEUE2).c $(HEADERS)
	$(CC) $(CFLAGS) -o $(PQUEUE2) $(PQUEUE2).c

clean:
	$(RM) $(TARGETS) *.o .*.dwo *.d
