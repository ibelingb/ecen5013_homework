CC = arm-buildroot-linux-uclibcgnueabihf-gcc
CFLAGS = -g -Wall -lrt -pthread

TARGET = pthread

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET) *.o .*.dwo *.d
