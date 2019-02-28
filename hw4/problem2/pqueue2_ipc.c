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
  char* logName = "mq2_ipc.log";
  char* mqName = "/mq_ipc";
  FILE* logFile = NULL;
  struct Payload sendPayload = {0};
  struct Payload rcvPayload = {0};
  mqd_t mqd;
  struct mq_attr mqAttr;
  unsigned int readPrio = 1;
  int status = 0;

  printf("Size: %d\n", sizeof(struct Payload));

  // Create/Open Message Queue
  mqAttr.mq_flags = 0;
  mqAttr.mq_maxmsg = 10;
  mqAttr.mq_msgsize = sizeof(struct Payload);
  mqAttr.mq_curmsgs = 0;

  mqd = mq_open(mqName, O_RDONLY, 0666, mqAttr);
  // TODO

  // Open Log file
  if ((logFile = fopen(logName, "w")) == NULL){
    printf("ERROR: Failed to open logfile for MessageQueue Process 2 - exiting.\n");
    return -1;
  }
  
  // Log current process info to log file
  printf("MessageQueue Process 2 Info:\nPID: {%d} | 1 FD open for MessageQueue.\n", getpid());
  fprintf(logFile, "[%s] MessageQueue Process 2 Info:\nPID: {%d} | 1 FD open for MessageQueue.\n", getTimestamp(), getpid());

  // --------------------------------------------------------------------------------
  // Read payload from MessageQueue

  mq_getattr(mqd, &mqAttr);
  // TODO: Add check for curMsgs > 0
  /*
  printf("flags: %ld | maxmsgs: %ld | msgsize: %ld | curmsgs: %ld\n", 
      mqAttr.mq_flags,
      mqAttr.mq_maxmsg,
      mqAttr.mq_msgsize,
      mqAttr.mq_curmsgs
      );
  */

  //status = mq_receive(mqd, (char *)&rcvPayload, sizeof(struct Payload) +1, &readPrio);
  status = mq_receive(mqd, (char *)&rcvPayload, MSG_SIZE+1, &readPrio);
  perror("Error: ");
  printf("Bytes read: %d\n", status);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);
  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  // --------------------------------------------------------------------------------
  // Send payloads to MessageQueue

  updatePayload(&sendPayload, 3, "", 0); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);

  updatePayload(&sendPayload, 0, "update", 6); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);

  updatePayload(&sendPayload, 0, "", 0); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);

  // Delay to demonstrate MessageQueue read to block
  sleep(2);

  updatePayload(&sendPayload, 0, "TESTING", 7); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);

  updatePayload(&sendPayload, 1, "", 0); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);


  // --------------------------------------------------------------------------------
  // Read payload from MessageQueue

  fprintf(logFile, "[%s] Payload Received -  Cmd {%d} | Msg {%s} | Len {%d}.\n",
          getTimestamp(), rcvPayload.cmd, rcvPayload.msg, rcvPayload.length);

  // --------------------------------------------------------------------------------
  // Send payloads to MessageQueue

  updatePayload(&sendPayload, 99, "Ten!!", 5); 
  fprintf(logFile, "[%s] Payload Sent -  Cmd {%d} | Msg {%s} | Len {%d}.\n", 
          getTimestamp(), sendPayload.cmd, sendPayload.msg, sendPayload.length);

  // --------------------------------------------------------------------------------
  // Cleanup
  fflush(logFile);
  fclose(logFile);
  mq_close(mqd);
}

/* ------------------------------------------------------------- */
