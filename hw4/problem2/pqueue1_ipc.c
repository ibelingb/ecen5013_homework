/* ECEN5013 - HW4
 * Date: 2/28/2019
 * Author: Brian Ibeling
 * About: C program to demonstrate IPC using Posix Message Queues between processes.
 * Resources: Utilized the following resourecs when developing the code contained in this file.
 *    - Linux Man Pages
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h> /* For file handling */
#include <string.h>
#include <mqueue.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "payload.h"

#define MSG_SIZE 8192
#define DELAY 2

/* Define static and global variables */
char* logName = "mq1_ipc.log";
char* mqName = "/mq_ipc";
FILE* logFile = NULL;
mqd_t mqd;

/* Define Function Prototypes */
void updatePayload(struct Payload* payload, unsigned char cmd, char* msg, unsigned char length);
void sigHandler(int sig);

/* ------------------------------------------------------------- */
int main() {
  struct Payload sendPayload = {0};
  struct Payload rcvPayload = {0};
  struct mq_attr mqAttr;
  struct sigaction sigAction;
  unsigned int readPrio = 1;

  // Register Signal Handler
  sigAction.sa_handler = sigHandler;
  sigAction.sa_flags = 0;
  sigaction(SIGINT, &sigAction, NULL);

  // Ensure MQs properly cleaned up before starting
  mq_unlink(mqName);

  // Create/Open Message Queue
  mqAttr.mq_flags = 0;
  mqAttr.mq_maxmsg = 10;
  mqAttr.mq_msgsize = MSG_SIZE;
  mqAttr.mq_curmsgs = 0;
  mqd = mq_open(mqName, O_CREAT | O_RDWR, 0666, &mqAttr);
  if(mqd == -1){
    printf("Message Queue Process 1 failed to create MQ - exiting.\n");
    perror("Error: \n");
    return -1;
  }

  // Open Log file
  if ((logFile = fopen(logName, "w")) == NULL){
    printf("ERROR: Failed to open logfile for MessageQueue Process 1 - exiting.\n");
    return -1;
  } 

  // Log current process info to log file
  printf("MessageQueue Process 1 Info:\nPID: {%d} | 1 FD open for MessageQueue, Log file open.\n", getpid());
  fprintf(logFile, "[%s] MessageQueue Process 1 Info:\nPID: {%d} | 1 FD open for MessageQueue, Log file open.\n", getTimestamp(), getpid());

  // --------------------------------------------------------------------------------
  // Send payloads to MessageQueue

  updatePayload(&sendPayload, 1, "", 0);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  mq_send(mqd, (char *)&sendPayload, sizeof(struct Payload)+1, readPrio);
  updatePayload(&sendPayload, 0, "test", 4);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  mq_send(mqd, (char *)&sendPayload, sizeof(struct Payload)+1, readPrio);
  updatePayload(&sendPayload, 1, "", 0);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  mq_send(mqd, (char *)&sendPayload, sizeof(struct Payload)+1, readPrio);

  // --------------------------------------------------------------------------------
  // Read payload from MessageQueue

  // Short delay to allow other procss to receive messages sent and P2 to start
  sleep(DELAY*2);

  mq_receive(mqd, (char *)&rcvPayload, MSG_SIZE+1, &readPrio);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  mq_receive(mqd, (char *)&rcvPayload, MSG_SIZE+1, &readPrio);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  mq_receive(mqd, (char *)&rcvPayload, MSG_SIZE+1, &readPrio);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  mq_receive(mqd, (char *)&rcvPayload, MSG_SIZE+1, &readPrio);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  mq_receive(mqd, (char *)&rcvPayload, MSG_SIZE+1, &readPrio);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  sleep(DELAY);
  // --------------------------------------------------------------------------------
  // Send payloads to MessageQueue

  updatePayload(&sendPayload, 1, "Testing Write 9", 15);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  mq_send(mqd, (char *)&sendPayload, sizeof(struct Payload)+1, readPrio);

  sleep(DELAY);
  // --------------------------------------------------------------------------------
  // Read payload from MessageQueue

  mq_receive(mqd, (char *)&rcvPayload, MSG_SIZE+1, &readPrio);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  sleep(DELAY);

  // --------------------------------------------------------------------------------
  // Alternate writing and reading
  updatePayload(&sendPayload, 0, "P1 Test1", 8);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  mq_send(mqd, (char *)&sendPayload, sizeof(struct Payload)+1, readPrio);
  sleep(DELAY);

  mq_receive(mqd, (char *)&rcvPayload, MSG_SIZE+1, &readPrio);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  sleep(DELAY);

  updatePayload(&sendPayload, 0, "P1 Test2", 8);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  mq_send(mqd, (char *)&sendPayload, sizeof(struct Payload)+1, readPrio);
  sleep(DELAY);

  mq_receive(mqd, (char *)&rcvPayload, MSG_SIZE+1, &readPrio);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  sleep(DELAY);

  updatePayload(&sendPayload, 0, "P1 Test3", 8);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  mq_send(mqd, (char *)&sendPayload, sizeof(struct Payload)+1, readPrio);
  sleep(DELAY);

  mq_receive(mqd, (char *)&rcvPayload, MSG_SIZE+1, &readPrio);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  sleep(DELAY);

  updatePayload(&sendPayload, 0, "P1 Test4", 8);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  mq_send(mqd, (char *)&sendPayload, sizeof(struct Payload)+1, readPrio);
  sleep(DELAY);

  mq_receive(mqd, (char *)&rcvPayload, MSG_SIZE+1, &readPrio);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  sleep(DELAY);

  updatePayload(&sendPayload, 0, "P1 Test5", 8);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  mq_send(mqd, (char *)&sendPayload, sizeof(struct Payload)+1, readPrio);
  sleep(DELAY);

  mq_receive(mqd, (char *)&rcvPayload, MSG_SIZE+1, &readPrio);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  sleep(DELAY);

  updatePayload(&sendPayload, 0, "P1 Test6", 8);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  mq_send(mqd, (char *)&sendPayload, sizeof(struct Payload)+1, readPrio);

  // --------------------------------------------------------------------------------
  // Cleanup
  printf("[%s] Message Queue Process 1 Complete.\n", getTimestamp());
  fprintf(logFile, "[%s] Message Queue Process 1 Complete.\n", getTimestamp());

  fflush(logFile);
  fclose(logFile);
  mq_close(mqd);
  mq_unlink(mqName);
}

// --------------------------------------------------------------------------------
void sigHandler(int sig){
  // Ensure logFile is closed before opening
  fflush(logFile);
  fclose(logFile);

  // Open log to capture that signal event has occurred
  if ((logFile = fopen(logName, "w")) == NULL){
    printf("ERROR: Failed to open logfile in MQ Process 1 sigHandler() - Exiting and cleaning resources.\n");
  }

  printf("[%s] SIGINT signal received! Killing MQ 1 process {%d}\n.", getTimestamp(), getpid());
  fprintf(logFile, "[%s] SIGINT signal received! Killing MQ 1 process {%d}\n.", getTimestamp(), getpid());

  // cleanup resources
  fflush(logFile);
  fclose(logFile);
  mq_close(mqd);
  mq_unlink(mqName);

  exit(1);
}
