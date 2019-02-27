/* ECEN5013 - HW4
 * Date: 2/28/2019
 * Author: Brian Ibeling
 * About: C program to demonstrate IPC using pipes between processes.
 * Resources: Utilized the following resourecs when developing the code contained in this file.
 *    - 
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> /* For file handling */
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "payload.h"

/* Define static and global variables */

/* Define Function Prototypes */
void updatePayload(struct Payload* payload, unsigned char cmd, char* msg, unsigned char length);

/* ------------------------------------------------------------- */
int main() {
  char* mFifo = "/tmp/ipc_fifo.txt";
  char* logName = "pipe1_ipc.log";
  FILE* logFile = NULL;
  struct Payload sendPayload = {0};
  struct Payload rcvPayload = {0};
  int fd;

  // Create FIFO
  mkfifo(mFifo, 0666);

  // Open Log file
  if ((logFile = fopen(logName, "w")) == NULL){
    printf("ERROR: Failed to open logfile for FIFO Process 2 - exiting.\n");
    return -1;
  } 

  // Log current process info to log file
  printf("Pipe Process 1 Info:\nPID: {%d} | 1 FD open for FIFO.\n", getpid());
  fprintf(logFile, "[%s] Pipe Process 1 Info:\nPID: {%d} | 1 FD open for FIFO.\n", getTimestamp(), getpid());

  // --------------------------------------------------------------------------------
  // Send payloads to FIFO
  fd = open(mFifo, O_WRONLY);
  if(fd == -1){
    printf("Failed to open FIFO at {%s} - exiting.\n", mFifo);
    return -1;
  }

  sleep(10);

  updatePayload(&sendPayload, 1, "", 0);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  write(fd, &sendPayload, sizeof(struct Payload));
  updatePayload(&sendPayload, 0, "test", 4);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  write(fd, &sendPayload, sizeof(struct Payload));
  updatePayload(&sendPayload, 1, "", 0);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  write(fd, &sendPayload, sizeof(struct Payload));
  close(fd);

  // --------------------------------------------------------------------------------
  // Read payload from FIFO
  fd = open(mFifo, O_RDONLY);
  if(fd == -1){
    printf("Failed to open FIFO at {%s} - exiting.\n", mFifo);
    return -1;
  }

  read(fd, &rcvPayload, sizeof(struct Payload));
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  read(fd, &rcvPayload, sizeof(struct Payload));
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  read(fd, &rcvPayload, sizeof(struct Payload));
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  read(fd, &rcvPayload, sizeof(struct Payload));
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  read(fd, &rcvPayload, sizeof(struct Payload));
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  close(fd);
  // --------------------------------------------------------------------------------
  // Send payloads to FIFO
  fd = open(mFifo, O_WRONLY);
  if(fd == -1){
    printf("Failed to open FIFO at {%s} - exiting.\n", mFifo);
    return -1;
  }

  updatePayload(&sendPayload, 1, "Testing Write 9", 15);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  write(fd, &sendPayload, sizeof(struct Payload));

  close(fd);
  // --------------------------------------------------------------------------------
  // Read payload from FIFO
  fd = open(mFifo, O_RDONLY);
  if(fd == -1){
    printf("Failed to open FIFO at {%s} - exiting.\n", mFifo);
    return -1;
  }

  read(fd, &rcvPayload, sizeof(struct Payload));
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  close(fd);
  // --------------------------------------------------------------------------------

  // Cleanup
  fflush(logFile);
  fclose(logFile);
  unlink(mFifo);
}

/* ------------------------------------------------------------- */
