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
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "payload.h"

/* Define static and global variables */
char* logName = "pipe2_ipc.log";
char* mFifo = "/tmp/ipc_fifo.txt";
FILE* logFile = NULL;
int fd;

/* Define Function Prototypes */
void updatePayload(struct Payload* payload, unsigned char cmd, char* msg, unsigned char length);
void sigHandler(int sig);

/* ------------------------------------------------------------- */
int main() {
  struct Payload sendPayload = {0};
  struct Payload rcvPayload = {0};
  struct sigaction sigAction;

  // Register Signal Handler
  sigAction.sa_handler = sigHandler;
  sigAction.sa_flags = 0;
  sigaction(SIGINT, &sigAction, NULL);

  // NOTE: FIFO should be created by pipe1_ipc process; continuing

  // Open Log file
  if ((logFile = fopen(logName, "w")) == NULL){
    printf("ERROR: Failed to open logfile for FIFO Process 2 - exiting.\n");
    return -1;
  }
  
  // Log current process info to log file
  printf("Pipe Process 2 Info - PID: {%d} | 1 FD open for FIFO, Log file open.\n", getpid());
  fprintf(logFile, "[%s] Pipe Process 2 Info - PID: {%d} | 1 FD open for FIFO, Log file open.\n", getTimestamp(), getpid());

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

  close(fd);
  // --------------------------------------------------------------------------------
  // Send payloads to FIFO
  fd = open(mFifo, O_WRONLY);
  if(fd == -1){
    printf("Failed to open FIFO at {%s} - exiting.\n", mFifo);
    return -1;
  }

  updatePayload(&sendPayload, 3, "", 0); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  write(fd, &sendPayload, sizeof(struct Payload));

  updatePayload(&sendPayload, 0, "update", 6); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  write(fd, &sendPayload, sizeof(struct Payload));

  updatePayload(&sendPayload, 0, "", 0); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  write(fd, &sendPayload, sizeof(struct Payload));

  // Delay to demonstrate FIFO read to block
  sleep(2);

  updatePayload(&sendPayload, 0, "TESTING", 7); 
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

  close(fd);
  // --------------------------------------------------------------------------------
  // Send payload to FIFO
  fd = open(mFifo, O_WRONLY);
  if(fd == -1){
    printf("Failed to open FIFO at {%s} - exiting.\n", mFifo);
    return -1;
  }

  updatePayload(&sendPayload, 99, "Ten!!", 5); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  write(fd, &sendPayload, sizeof(struct Payload));

  updatePayload(&sendPayload, 1, "P2 Test1", 8); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  write(fd, &sendPayload, sizeof(struct Payload));

  updatePayload(&sendPayload, 0, "P2 Test2", 8); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  write(fd, &sendPayload, sizeof(struct Payload));

  updatePayload(&sendPayload, 1, "P2 Test3", 8); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  write(fd, &sendPayload, sizeof(struct Payload));

  updatePayload(&sendPayload, 0, "P2 Test4", 8); 
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

  read(fd, &rcvPayload, sizeof(struct Payload));
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  close(fd);
  // --------------------------------------------------------------------------------
  // Cleanup
  printf("[%s] Pipe Process 2 Complete.\n", getTimestamp());
  fprintf(logFile, "[%s] Pipe Process 2 Complete.\n", getTimestamp());

  fflush(logFile);
  fclose(logFile);
  close(fd);
  unlink(mFifo);
}

// --------------------------------------------------------------------------------
void sigHandler(int sig){
  // Ensure logFile is closed before opening
  fflush(logFile);
  fclose(logFile);

  // Open log to capture that signal event has occurred
  if ((logFile = fopen(logName, "w")) == NULL){
    printf("ERROR: Failed to open logfile in FIFO Process 2 sigHandler() - Exiting and cleaning resources.\n");
  } 

  printf("[%s] SIGINT signal received! Killing Pipe 2 process {%d}\n.", getTimestamp(), getpid());
  fprintf(logFile, "[%s] SIGINT signal received! Killing Pipe 2 process {%d}\n.", getTimestamp(), getpid());

  // cleanup resources
  fflush(logFile);
  fclose(logFile);
  close(fd);
  unlink(mFifo);

  exit(1);
}

