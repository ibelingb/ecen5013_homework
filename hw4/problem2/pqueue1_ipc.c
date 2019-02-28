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
#include <sys/types.h>
#include <sys/stat.h>

#include "payload.h"

#define MSG_SIZE 8192

/* Define static and global variables */

/* Define Function Prototypes */
void updatePayload(struct Payload* payload, unsigned char cmd, char* msg, unsigned char length);

/* ------------------------------------------------------------- */
int main() {
  char* logName = "mq1_ipc.log";
  char* mqName = "/mq_ipc";
  FILE* logFile = NULL;
  struct Payload sendPayload = {0};
  struct Payload rcvPayload = {0};
  mqd_t mqd;
  struct mq_attr mqAttr;
  unsigned int readPrio = 1;
  int status = 0;

  // Create/Open Message Queue, with attributes
  mqd = mq_open(mqName, O_CREAT | O_RDWR, 0666);
  // TODO

  // Open Log file
  if ((logFile = fopen(logName, "w")) == NULL){
    printf("ERROR: Failed to open logfile for MessageQueue Process 1 - exiting.\n");
    return -1;
  } 

  // Log current process info to log file
  // TODO
  printf("MessageQueue Process 1 Info:\nPID: {%d} | 1 FD open for MessageQueue.\n", getpid());
  fprintf(logFile, "[%s] MessageQueue Process 1 Info:\nPID: {%d} | 1 FD open for MessageQueue.\n", getTimestamp(), getpid());

  // --------------------------------------------------------------------------------
  // Send payloads to MessageQueue

  updatePayload(&sendPayload, 1, "", 0);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  mq_send(mqd, (char *)&sendPayload, sizeof(struct Payload)+1, readPrio);

  updatePayload(&sendPayload, 0, "test", 4);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);
  updatePayload(&sendPayload, 1, "", 0);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);

  // --------------------------------------------------------------------------------
  // Read payload from MessageQueue

  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  // --------------------------------------------------------------------------------
  // Send payloads to MessageQueue

  updatePayload(&sendPayload, 1, "Testing Write 9", 15);
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);

  // --------------------------------------------------------------------------------
  // Read payload from MessageQueue

  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  // --------------------------------------------------------------------------------

  sleep(5);

  // Cleanup
  fflush(logFile);
  fclose(logFile);
  mq_close(mqd);
  mq_unlink(mqName);
}

